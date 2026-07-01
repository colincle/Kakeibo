# Kakeibo

A desktop budgeting app inspired by the Japanese *kakeibo* (家計簿) — a method of
tracking household spending by category and reflecting on it each month — here
implemented with an envelope-budgeting model. You give every yen a job by
dividing your income across labelled envelopes, import your real bank
transactions, and watch each envelope fill up or drain over the month.

Written in **C++20** with **Qt 6**. The interface is bilingual (French / 日本語).

## Background

This is a personal tool, built for my own use and shared publicly as a portfolio
piece rather than a product. I made it to manage one household's budget across
two currencies — euros and yen — using the kakeibo approach.

Because it only ever needed to fit one person, the scope is intentionally narrow:

- single-user desktop app; all data is stored locally (no accounts, no backend);
- the interface is French/Japanese only;
- the statement importers handle just the banks I use (Rakuten Bank, Rakuten
  Card, Mitsubishi UFJ).

You're welcome to read, fork, or adapt it, but it isn't meant to work
out-of-the-box for a different setup.

## Features

- **Envelope budgeting** — create envelopes with a monthly goal and a ceiling,
  mark some as savings, and track how full each one is at a glance.
- **Bank statement import** — paste a raw statement and it is parsed
  automatically. Supported formats: Rakuten Bank, Rakuten Card (credit), and
  Mitsubishi UFJ. Full-width Japanese digits are normalised to ASCII.
- **Automatic categorisation** — expenses are matched to envelopes by remembered
  description types; unknown ones prompt you once and are remembered.
- **Dispatch page** — distribute your income across envelopes, evenly or to
  each goal, with per-envelope locking and full undo/redo before you apply.
- **History & statistics** — filter expenses by envelope and date range, and see
  per-month / per-year averages.
- **EUR / JPY conversion** — every amount is shown in yen and euros using live
  European Central Bank reference rates (no API key required).
- **Backups & cloud snapshot** — daily local backups (30 kept) and an optional
  image snapshot of selected envelopes written to a sync folder.

## Building

Requires CMake ≥ 3.16 and Qt 6 (Core, Gui, Widgets, Network, Concurrent).

```sh
cmake -S . -B build
cmake --build build
./build/Kakeibo
```

Or use the helper:

```sh
./scripts/run.sh
```

### Tests

The bank-statement parser has unit tests (pure logic, no Qt required):

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

### macOS app bundle

```sh
./scripts/bundle.sh   # produces Kakeibo.app (requires macdeployqt on PATH)
```

## Data location

State is stored as JSON under `$XDG_DATA_HOME/kakeibo` (or
`~/.local/share/kakeibo`): `enveloppes.json`, `specialEnveloppes.json`, and the
backup/cloud path config.

## Project layout

```
include/core   data model, parsing, persistence, backups, cloud sync
include/ui     the Qt views (envelopes, history, stats, dispatch, import…)
include/uiElements  small reusable widgets
src/           implementations mirroring include/
external/json  nlohmann/json (single header)
scripts/       run, bundle and formatting helpers
```

## License

MIT — see [LICENSE](LICENSE).
