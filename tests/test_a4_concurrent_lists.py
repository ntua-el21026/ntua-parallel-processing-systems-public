"""Verify the six independent list implementations on a GNU/Linux host."""
import os
from pathlib import Path
import shlex
import subprocess
from tempfile import TemporaryDirectory
import unittest


PROJECT = Path(__file__).resolve().parents[1]
LISTS = PROJECT / "a4" / "conc_ll"
VARIANTS = ("serial", "cgl", "fgl", "opt", "lazy", "nb")


class ConcurrentListTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        temporary = TemporaryDirectory()
        cls.addClassCleanup(temporary.cleanup)
        build = Path(temporary.name)
        cls.binaries = {}
        command = shlex.split(os.environ.get("CC", "cc"))
        command.extend(["-std=gnu11", "-Wall", "-Wextra", "-Werror", "-pthread", "-O2"])
        command.extend(shlex.split(os.environ.get("CFLAGS", "")))
        command.extend(["-I", str(LISTS)])
        for variant in VARIANTS:
            binary = build / variant
            subprocess.run(
                [*command, str(PROJECT / "tests/a4_lists_probe.c"),
                 str(LISTS / f"ll/ll_{variant}.c"), "-o", str(binary)],
                check=True, timeout=30,
            )
            cls.binaries[variant] = binary
        cls.support = build / "support"
        subprocess.run(
            [*command, str(PROJECT / "tests/a4_support_probe.c"),
             str(LISTS / "lib/aff.c"), "-o", str(cls.support)],
            check=True, timeout=30,
        )

    def run_probe(self, program, *arguments, environment=None, status=0):
        result = subprocess.run(
            [str(program), *arguments], capture_output=True, text=True,
            env=environment, timeout=20,
        )
        self.assertEqual(result.returncode, status, result.stderr)
        return result.stdout

    def test_sequential_reference_set(self):
        for variant, program in self.binaries.items():
            with self.subTest(variant=variant):
                self.assertEqual(self.run_probe(program, "sequential"), "-1 8 20\n")

    def test_competing_same_key_changes(self):
        for variant in VARIANTS[1:]:
            with self.subTest(variant=variant):
                self.run_probe(self.binaries[variant], "competing")

    def test_interleaved_disjoint_key_changes(self):
        for variant in VARIANTS[1:]:
            with self.subTest(variant=variant):
                self.run_probe(self.binaries[variant], "disjoint")

    def test_affinity_defaults_and_oversubscription(self):
        environment = os.environ.copy()
        environment.pop("MT_CONF", None)
        cpu = min(os.sched_getaffinity(0))
        self.assertEqual(self.run_probe(self.support, environment=environment), f"MT_CONF={cpu}\n")
        environment["MT_CONF"] = f"  {cpu} , {cpu} "
        self.assertEqual(
            self.run_probe(self.support, environment=environment), f"MT_CONF={cpu},{cpu}\n"
        )

    def test_invalid_affinity_configuration(self):
        cpu = min(os.sched_getaffinity(0))
        for invalid in ("-1", "not-a-cpu", f"{cpu},", "100000", ",".join([str(cpu)] * 129)):
            with self.subTest(value=invalid):
                environment = dict(os.environ, MT_CONF=invalid)
                self.run_probe(self.support, environment=environment, status=1)


if __name__ == "__main__":
    unittest.main()
