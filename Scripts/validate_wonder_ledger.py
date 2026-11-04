"""Validate the shared Wonder World ledger JSON structure and numeric values."""

import argparse
import json
import math
from pathlib import Path


DEFAULT_LEDGER = (
    Path(__file__).resolve().parents[1]
    / "WonderPipeline"
    / "02_Shared_Game_Core"
    / "State"
    / "WonderLedgerState.json"
)

REQUIRED_COLUMNS = (
    "column_1_gross_revenue",
    "column_2_operating_expenses",
    "column_3_net_reserve_and_upgrades",
)


def validate_ledger(path: Path) -> None:
    try:
        with path.open(encoding="utf-8") as ledger_file:
            data = json.load(ledger_file)
    except (OSError, json.JSONDecodeError) as error:
        raise ValueError(f"Unable to read valid ledger JSON at {path}: {error}") from error

    if not isinstance(data, dict):
        raise ValueError("Ledger root must be a JSON object")
    if not isinstance(data.get("schema_version"), str) or not data["schema_version"]:
        raise ValueError("Ledger must declare a non-empty schema_version")
    if not isinstance(data.get("tower_name"), str) or not data["tower_name"]:
        raise ValueError("Ledger must declare a non-empty tower_name")
    if not isinstance(data.get("evon_hub_permission_granted"), bool):
        raise ValueError("evon_hub_permission_granted must be a boolean")

    columns = data.get("ledger_columns")
    if not isinstance(columns, dict):
        raise ValueError("ledger_columns must be a JSON object")
    for column_name in REQUIRED_COLUMNS:
        column = columns.get(column_name)
        if not isinstance(column, dict):
            raise ValueError(f"Missing or invalid ledger column: {column_name}")

        for field_name, value in column.items():
            if isinstance(value, bool):
                continue
            if isinstance(value, (int, float)) and not math.isfinite(value):
                raise ValueError(
                    f"{column_name}.{field_name} must be a finite number"
                )

    print(
        "VALIDATED Wonder World ledger",
        data["schema_version"],
        "-",
        data["tower_name"],
    )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ledger", type=Path, default=DEFAULT_LEDGER)
    args = parser.parse_args()
    validate_ledger(args.ledger)


if __name__ == "__main__":
    main()
