import json
from random import uniform
from datetime import datetime, timedelta

def generate_hourly_data():
    return {
        "H": f"{uniform(20.0, 80.0):.2f}",
        "R": f"{uniform(0.0, 20.0):.2f}",
        "T": f"{uniform(20.0, 38.0):.2f}"
    }

def generate_daily_data(date_str):
    return {
        str(hour): generate_hourly_data() for hour in range(24)
    }

def generate_datalog(start_date, days):
    datalog = {}
    date_format = "%Y-%m-%d"
    start_date = datetime.strptime(start_date, date_format)
    for i in range(days):
        current_date = start_date + timedelta(days=i)
        date_str = current_date.strftime(date_format)
        datalog[date_str] = generate_daily_data(date_str)
    return datalog

data = {
    "data": {
        "H": "70.00",
        "L": "1731336572",
        "R": "799.50",
        "T": "30.40"
    }
}

# Generate data for the next 5 days starting from 2024-11-11
start_date = "2024-11-8"
days = 7
data["datalog"] = generate_datalog(start_date, days)

# Save data to JSON file
with open("output.json", "w") as json_file:
    json.dump(data, json_file, indent=2)

print("Data has been saved to output.json")
