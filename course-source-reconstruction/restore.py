#!/usr/bin/env python3
"""Assemble student changes with locally obtained, verified course source."""
import argparse
import hashlib
import json
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('base_directory', type=Path)
parser.add_argument('output_directory', type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parent
plan = json.loads((root / 'overlay.json').read_text())
outputs = {}
for change in plan:
    base = args.base_directory / change['base_path']
    data = base.read_bytes()
    if hashlib.sha256(data).hexdigest() != change['base_sha256']:
        parser.error(f'Incorrect course edition for {base}')
    lines = data.decode('utf-8').splitlines(keepends=True)
    combined = []
    for token in change['segments']:
        if 'copy' in token:
            start, stop = token['copy']
            combined.extend(lines[start:stop])
        else:
            combined.extend(token['student_lines'])
    generated = ''.join(combined).encode('utf-8')
    if hashlib.sha256(generated).hexdigest() != change['reconstructed_sha256']:
        parser.error(f'Unexpected reconstruction for {change["path"]}')
    destination = args.output_directory / change['path']
    if destination.exists() or destination.is_symlink():
        parser.error(f'Existing file at {destination}')
    if any(p.is_symlink() for p in destination.parents):
        parser.error(f'Symlink in output path {destination}')
    outputs[destination] = generated
for destination, generated in outputs.items():
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(generated)
