#!/usr/bin/env python3
"""
Regenerate the Sanuli word arrays inside ../Game_Sanuli.h.

Inputs (one UPPERCASE word per line, UTF-8):
  tools/common-words-5.txt  -> possible ANSWERS
  tools/full-words-5.txt    -> all ACCEPTED guesses (must be a superset of answers)

Output: replaces the block between the marker comments in Game_Sanuli.h with:
  answer_list[]  = answers (also valid guesses)
  extra_list[]   = accepted-only words (full minus common, no duplication)

Run from anywhere:  python3 tools/gen_sanuli_words.py
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
HEADER = os.path.join(HERE, "..", "Game_Sanuli.h")
COMMON = os.path.join(HERE, "common-words-5.txt")
FULL = os.path.join(HERE, "full-words-5.txt")

BEGIN = "// === BEGIN GENERATED WORDLISTS (tools/gen_sanuli_words.py) ==="
END = "// === END GENERATED WORDLISTS ==="

ALLOWED = set("ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖ")


def load(path):
    with open(path, encoding="utf-8") as f:
        words = [ln.strip() for ln in f if ln.strip()]
    seen, out = set(), []
    for w in words:                     # drop accidental duplicates, keep order
        if w not in seen:
            seen.add(w)
            out.append(w)
    return out


def validate(name, words):
    for w in words:
        if len(w) != 5:
            sys.exit(f"ERROR: {name}: '{w}' is not 5 letters")
        bad = [c for c in w if c not in ALLOWED]
        if bad:
            sys.exit(f"ERROR: {name}: '{w}' has unsupported char(s) {bad} "
                     f"(only A-Z, Ä, Ö are typable on the device)")


def emit_array(name, words):
    lines = [f"const char* const {name}[] PROGMEM = {{"]
    for i in range(0, len(words), 10):
        chunk = ", ".join(f'"{w}"' for w in words[i:i + 10])
        lines.append(f"  {chunk},")
    # drop trailing comma on the last data line, then close
    lines[-1] = lines[-1].rstrip(",")
    lines.append("};")
    return "\n".join(lines)


def main():
    common = load(COMMON)
    full = load(FULL)
    validate("common", common)
    validate("full", full)

    cset = set(common)
    missing = [w for w in common if w not in set(full)]
    if missing:
        sys.exit(f"ERROR: {len(missing)} answer(s) not in accepted list: {missing[:10]}")

    extra = [w for w in full if w not in cset]  # accepted-only, preserves file order

    block = "\n".join([
        BEGIN,
        f"// answers: {len(common)}  |  accepted-only: {len(extra)}  |  total accepted: {len(full)}",
        f"// Regenerate with: python3 tools/gen_sanuli_words.py",
        "",
        "// Possible answers (also valid guesses).",
        emit_array("answer_list", common),
        "",
        "// Extra accepted guesses that are never the answer.",
        emit_array("extra_list", extra),
        END,
    ])

    with open(HEADER, encoding="utf-8") as f:
        src = f.read()

    if BEGIN not in src or END not in src:
        sys.exit("ERROR: markers not found in Game_Sanuli.h. "
                 "Add the BEGIN/END marker comments first.")

    pre = src[:src.index(BEGIN)]
    post = src[src.index(END) + len(END):]
    with open(HEADER, "w", encoding="utf-8") as f:
        f.write(pre + block + post)

    print(f"OK: answers={len(common)} extra={len(extra)} total={len(full)}")
    print(f"Wrote {os.path.normpath(HEADER)}")


if __name__ == "__main__":
    main()
