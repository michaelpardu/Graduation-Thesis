import random
import time
from datetime import datetime, timedelta

# Define start and end dates
start_date = datetime.strptime('20/8/2024', '%d/%m/%Y')
end_date = datetime.strptime('30/8/2024', '%d/%m/%Y')

data = []
current_date = start_date

while current_date <= end_date:
    if current_date.weekday() < 5:  # Check if it's a weekday
        if random.choice([True, False]):
            # Morning: 8AM to 11AM
            random_hour = random.randint(8, 10)
        else:
            # Afternoon: 1PM to 5PM
            random_hour = random.randint(13, 16)
            
        random_minute = random.randint(0, 59)
        random_second = random.randint(0, 59)
        epoch_time = int(datetime(
            current_date.year, current_date.month, current_date.day,
            random_hour, random_minute, random_second
        ).timestamp())
        random_time = random.randint(1000, 7000)
        data.append(f'"{epoch_time}":"{random_time}"')
            
    current_date += timedelta(days=1)

# Join data elements with commas and save to a text file
output = ','.join(data)

with open('data.txt', 'w') as f:
    f.write(output)

print("Text file generated successfully!")
