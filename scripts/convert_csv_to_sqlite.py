import argparse
import csv
from datetime import datetime
import re
import sqlite3

parser = argparse.ArgumentParser(description='Copies map history from the old CSV format into an existing SQLite db, the previews should be copied independently')
parser.add_argument("input", help="path to the CSV file")
parser.add_argument("output", help="path to the SQLite file")

args = parser.parse_args()

rows = list()
with open(args.input, mode='r') as csvfile:
    map_history = csv.DictReader(csvfile, fieldnames=["workshop_id", "map_mame", "played_at", "preview_path"])
    next(map_history, None)
    for row in map_history:
        rows.append(row)

con = sqlite3.connect(args.output, autocommit=False)
cur = con.cursor()

def convert_timestamp(ts_str):
    # Trim fractional seconds to max 6 digits
    ts = re.sub(r'(\.\d{6})\d+', r'\1', ts_str)

    formats = [
        "%Y-%m-%d %H:%M:%S.%f",
        "%d-%m-%Y %H:%M:%S",
    ]
    for fmt in formats:
        try:
            return datetime.strptime(ts, fmt)
        except ValueError:
            continue
    raise ValueError(f"Unrecognized timestamp format: {ts}")

for map_entry in rows:
    try:
        print(map_entry)
        res = cur.execute("SELECT EXISTS(SELECT 1 FROM maps WHERE workshop_id = ?);", (map_entry["workshop_id"],))
        does_exist = res.fetchone()[0]

        if does_exist == False:
            cur.execute("INSERT INTO maps (workshop_id, name, preview_path) VALUES (?, ?, ?);",
                (map_entry["workshop_id"], map_entry["map_mame"], map_entry["preview_path"]))
            
        dt = convert_timestamp(map_entry["played_at"])
        output_ts = dt.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]

        cur.execute("INSERT INTO session_history (workshop_id, played_at) VALUES (?, ?);",
            (map_entry["workshop_id"], output_ts))

    except Exception as err:
        print("Error while processing: {}\nRow: {}".format(err, str(map_entry)))
        break
else:
    con.commit()

con.close()
