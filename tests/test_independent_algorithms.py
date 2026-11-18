"""Independent algorithm references and compatibility contracts, with temporary builds."""
import ctypes
import os
from pathlib import Path
import random
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
FLOYD = ROOT / "a2" / "FW"
HEAT = ROOT / "a6" / "heat_transfer"
GENERATOR = ROOT / "a5" / "Helper_functions"
INF = (1 << 31) - 1
IntRows = ctypes.POINTER(ctypes.POINTER(ctypes.c_int))
DoubleRows = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))


def build(directory, *command):
    subprocess.run(command, cwd=directory, check=True, timeout=35, capture_output=True)


def invoke(directory, *command, **kwargs):
    return subprocess.run(command, cwd=directory, timeout=8, text=True,
                          capture_output=True, **kwargs)


def paths_by_relaxing_edges(matrix):
    """Repeated edge relaxation per start vertex, independent of pivot traversal."""
    n = len(matrix)
    answers = []
    for start in range(n):
        distances = [None] * n
        distances[start] = 0
        for _ in range(n - 1):
            before = distances.copy()
            for source, prefix in enumerate(before):
                if prefix is None:
                    continue
                for dest, weight in enumerate(matrix[source]):
                    candidate = prefix + weight
                    if weight != INF and (distances[dest] is None or candidate < distances[dest]):
                        distances[dest] = candidate
        answers.append([INF if cost is None else cost for cost in distances])
    return answers


def sparse_graph(order, seed):
    rng = random.Random(seed)
    potential = [rng.randrange(-9, 10) for _ in range(order)]
    # A potential difference telescopes around cycles, so every cycle has a
    # nonnegative weight even though some individual edges are negative.
    return [
        [0 if src == dst else rng.randrange(0, 13) + potential[dst] - potential[src]
         if rng.random() < .53 else INF for dst in range(order)]
        for src in range(order)
    ]


class FloydWarshallTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.home = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.home.cleanup)
        cls.directory = Path(cls.home.name)
        for name in ("Makefile", "fw_reference.h", "fw.c", "fw_sr.c", "fw_sr_p.c",
                     "fw_tiled.c", "util.h", "util.c"):
            shutil.copyfile(FLOYD / name, cls.directory / name)
        build(cls.directory, "make", "all")
        library_path = cls.directory / "libreference.so"
        build(cls.directory, "gcc", "-std=gnu11", "-Wall", "-Wextra", "-Werror",
              "-fPIC", "-shared", "-DFW_REFERENCE_NO_MAIN",
              "fw.c", "fw_sr.c", "fw_tiled.c", "-o", library_path)
        cls.library = ctypes.CDLL(str(library_path))

    def compare(self, matrix, expected, blocks):
        n = len(matrix)
        row_type = ctypes.c_int * n
        for name in ("fw_plain", "fw_recursive", "fw_tiled"):
            function = getattr(self.library, name)
            function.argtypes = [IntRows, ctypes.c_int, ctypes.c_int]
            function.restype = None
            for block in blocks:
                with self.subTest(solver=name, block=block, order=n):
                    rows = [row_type(*values) for values in matrix]
                    ptrs = (ctypes.POINTER(ctypes.c_int) * n)(*rows)
                    function(ptrs, n, block)
                    self.assertEqual([list(row) for row in rows], expected)

    def test_negative_edges_disjoint_pairs_and_zero_weight(self):
        graph = [[0, 2, INF, 12, INF], [INF, 0, -3, INF, INF],
                 [INF, INF, 0, 0, INF], [4, INF, INF, 0, INF],
                 [INF, INF, INF, INF, 0]]
        self.compare(graph, paths_by_relaxing_edges(graph), (1, 2, 3, 8))

    def test_odd_and_partial_tile_dimensions(self):
        for order in (1, 2, 3, 5, 8, 11):
            for seed in (2, 41, 556):
                with self.subTest(order=order, seed=seed):
                    graph = sparse_graph(order, seed)
                    self.compare(graph, paths_by_relaxing_edges(graph), (1, 2, 4, 32))

    def test_integer_overflow_never_becomes_a_shortcut(self):
        graph = [[0, INF - 1, 11], [INF, 0, INF - 1], [INF, INF, 0]]
        self.compare(graph, graph, (1, 2, 3, 8))

    def test_benchmark_record_format_and_retained_parallel_target(self):
        for executable, arguments, prefix in (
            ("fw", ("5",), "FW,5,"),
            ("fw_sr", ("5", "2"), "FW_SR,5,2,"),
            ("fw_tiled", ("5", "2"), "FW_TILED,5,2,"),
            ("fw_sr_p", ("8", "2"), "FW_SR,8,2,"),
        ):
            with self.subTest(executable=executable):
                result = invoke(self.directory, f"./{executable}", *arguments,
                                env={**os.environ, "OMP_NUM_THREADS": "2"})
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertRegex(result.stdout, rf"^{prefix}\d+\.\d{{4}}\n$")

    def test_invalid_benchmark_dimensions_fail_before_allocation(self):
        for executable, arguments in (
            ("fw", ("-5",)),
            ("fw", ("2147483647",)),
            ("fw", ("9extra",)),
            ("fw_sr", ("5", "0")),
            ("fw_tiled", ("5",)),
        ):
            with self.subTest(executable=executable, arguments=arguments):
                result = invoke(self.directory, f"./{executable}", *arguments)
                self.assertNotEqual(result.returncode, 0)
                self.assertEqual(result.stdout, "")
                self.assertTrue(result.stderr)


class HostGenerationTests(unittest.TestCase):
    def test_gpp_interface_reproducibility_range_and_dimension_guards(self):
        with tempfile.TemporaryDirectory() as name:
            directory = Path(name)
            source = directory / "call-generator.cpp"
            source.write_text("""
#include <cassert>
#include <cmath>
#include <cstdlib>
#include "kmeans.h"

int main(int argc, char **argv)
{
    if (argc == 3) {
        std::free(dataset_generation(std::atoi(argv[1]), std::atoi(argv[2])));
        return 0;
    }

    double *first = dataset_generation(7, 3);
    double *second = dataset_generation(7, 3);
    const unsigned reference[] = {48271, 182605794, 1291394886, 1914720637};

    for (unsigned i = 0; i < 21; ++i) {
        assert(std::isfinite(first[i]) && first[i] >= 0.0 && first[i] < 1.0);
        assert(first[i] == second[i]);
        if (i < sizeof(reference) / sizeof(reference[0]))
            assert(first[i] == reference[i] / 2147483647.0);
    }
    assert(first[0] != first[1] && first[0] != first[3]);
    std::free(first);
    std::free(second);
}
""", encoding="utf-8")
            build(directory, "g++", "-Wall", "-Wextra", "-Werror", "-I", GENERATOR,
                  source, GENERATOR / "file_io.c", "-o", "generator")
            result = invoke(directory, "./generator")
            self.assertEqual(result.returncode, 0, result.stderr)
            for rows, cols in (("0", "1"), ("1", "-2"), ("2147483647", "2147483647")):
                with self.subTest(rows=rows, cols=cols):
                    result = invoke(directory, "./generator", rows, cols)
                    self.assertNotEqual(result.returncode, 0)
                    self.assertIn("Invalid K-means dataset dimensions", result.stderr)


class HeatGridTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.home = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.home.cleanup)
        cls.directory = Path(cls.home.name)
        cls.libraries = {}
        driver = cls.directory / "grid-contract.c"
        driver.write_text("""
#include <limits.h>
#include <string.h>
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (strcmp(argv[1], "zero-dimension") == 0) {
        free2d(allocate2d(0, 4));
    } else if (strcmp(argv[1], "overflow") == 0) {
        free2d(allocate2d(INT_MAX, INT_MAX));
    } else {
        double **grid = allocate2d(2, 3);
        init2d(grid, 2, 3);
        if (strcmp(argv[1], "stdout") == 0)
            print2d(grid, 2, 3);
        else
            fprint2d(argv[1], grid, 2, 3);
        free2d(grid);
    }
    return 0;
}
""", encoding="utf-8")
        for variant in ("mpi", "serial"):
            source = HEAT / variant / "utils.c"
            library = cls.directory / f"grid-{variant}.so"
            build(cls.directory, "gcc", "-std=gnu11", "-Wall", "-Wextra", "-Werror",
                  "-O2", "-fPIC", "-shared", source, "-o", library)
            lib = ctypes.CDLL(str(library))
            lib.allocate2d.argtypes = [ctypes.c_int, ctypes.c_int]
            lib.allocate2d.restype = DoubleRows
            lib.free2d.argtypes = [DoubleRows]
            lib.free2d.restype = None
            for func in (lib.init2d, lib.zero2d):
                func.argtypes = [DoubleRows, ctypes.c_int, ctypes.c_int]
                func.restype = None
            lib.converge.argtypes = [DoubleRows, DoubleRows, *([ctypes.c_int] * 4)]
            lib.converge.restype = ctypes.c_int
            lib.max.argtypes = [ctypes.c_double, ctypes.c_double]
            lib.max.restype = ctypes.c_double
            cls.libraries[variant] = lib
            build(cls.directory, "gcc", "-std=gnu11", "-Wall", "-Wextra", "-Werror",
                  "-I", source.parent, driver, source, "-o", f"grid-{variant}")

    def test_contiguous_storage_logical_boundary_and_padding(self):
        for variant, lib in self.libraries.items():
            with self.subTest(variant=variant):
                grid = lib.allocate2d(5, 7)
                try:
                    self.assertEqual(ctypes.addressof(grid[1].contents) -
                                     ctypes.addressof(grid[0].contents), 7 * ctypes.sizeof(ctypes.c_double))
                    grid[4][6] = 9.0
                    lib.init2d(grid, 4, 5)
                    for i in range(4):
                        for j in range(5):
                            self.assertEqual(grid[i][j], float(i in (0, 3) or j in (0, 4)))
                    self.assertEqual(grid[0][5], 0.0)
                    self.assertEqual(grid[4][6], 9.0)
                    lib.zero2d(grid, 5, 7)
                    self.assertTrue(all(grid[i][j] == 0 for i in range(5) for j in range(7)))
                finally:
                    lib.free2d(grid)
                lib.free2d(None)

    def test_inclusive_convergence_subdomain_and_nonfinite_values(self):
        for variant, lib in self.libraries.items():
            with self.subTest(variant=variant):
                old, new = lib.allocate2d(5, 7), lib.allocate2d(5, 7)
                try:
                    new[3][4] = 1.001e-6
                    self.assertEqual(lib.converge(old, new, 1, 3, 1, 4), 0)
                    self.assertEqual(lib.converge(old, new, 0, 2, 0, 6), 1)
                    new[3][4] = -1e-6
                    self.assertEqual(lib.converge(old, new, 1, 3, 1, 4), 1)
                    new[3][4] = float("nan")
                    self.assertEqual(lib.converge(old, new, 3, 3, 4, 4), 0)
                    new[3][4] = float("inf")
                    self.assertEqual(lib.converge(old, new, 3, 3, 4, 4), 0)
                    self.assertEqual(lib.converge(old, new, 3, 2, 1, 4), 1)
                    self.assertEqual(lib.converge(old, new, -1, 2, 1, 4), 0)
                    self.assertEqual(lib.max(-5, -2), -2)
                    self.assertEqual(lib.max(7, float("nan")), 7)
                finally:
                    lib.free2d(old)
                    lib.free2d(new)

    def test_file_format_stream_output_and_dimension_guards(self):
        for variant in self.libraries:
            with self.subTest(variant=variant):
                filename = f"grid-{variant}.txt"
                result = invoke(self.directory, f"./grid-{variant}", filename)
                self.assertEqual(result.returncode, 0, result.stderr)
                content = (self.directory / filename).read_text(encoding="utf-8")
                self.assertEqual([[float(v) for v in line.split()]
                                  for line in content.splitlines()], [[1.0] * 3] * 2)
                result = invoke(self.directory, f"./grid-{variant}", "stdout")
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(result.stdout, content)
                for arg in ("zero-dimension", "overflow", "missing-parent/out.txt"):
                    result = invoke(self.directory, f"./grid-{variant}", arg)
                    self.assertNotEqual(result.returncode, 0)
                    self.assertTrue(result.stderr)


if __name__ == "__main__":
    unittest.main()
