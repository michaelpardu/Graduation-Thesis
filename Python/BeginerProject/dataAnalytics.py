import pandas as pd
from datetime import datetime

data = {
          "UsedTime": {
            "1724146415": "1020",
            "1724232916": "4680",
            "1724297345": "3512",
            "1724376655": "1870",
            "1724740475": "4004",
            "1724825786": "6331",
            "1724903210": "1870",
            "1725008867": "6935",
            "1725956143": "5836",
            "1726022558": "3837",
            "1726107555": "1714",
            "1726199069": "5045",
            "1726473841": "5959",
            "1726563606": "1448",
            "1726625279": "1795",
            "1726733392": "2107",
            "1726804790": "2810",
            "1727078038": "5256",
            "1727159502": "5528",
            "1727253142": "4271",
            "1727313054": "3269",
            "1727401885": "1053",
            "1727668343": "5965",
            "1727749771": "1931",
            "1727833790": "3979",
            "1727935405": "1236",
            "1728014048": "5917",
            "1728282939": "6711",
            "1728367429": "1532",
            "1728467147": "2181",
            "1728546455": "6299",
            "1728628167": "1457",
            "1728896871": "2561",
            "1728957262": "6343"
          }
}

# Chuyển đổi dữ liệu sang định dạng dễ xử lý hơn
used_time = data["UsedTime"]

# Tạo danh sách chứa các khoảng thời gian bật và tắt
time_intervals = []

for end_time, duration in used_time.items():
    end_time = int(end_time)
    duration = int(duration)
    start_time = end_time - duration
    start_time_human = datetime.fromtimestamp(start_time).strftime('%Y-%m-%d %H:%M:%S')
    end_time_human = datetime.fromtimestamp(end_time).strftime('%Y-%m-%d %H:%M:%S')
    time_intervals.append((start_time_human, end_time_human))

# Chuyển đổi danh sách sang DataFrame
df = pd.DataFrame(time_intervals, columns=['Start Time', 'End Time'])

# Tách cột 'Start Time' thành ngày và thời gian
df['Start Date'] = pd.to_datetime(df['Start Time']).dt.date
df['Start Hour:Minute'] = pd.to_datetime(df['Start Time']).dt.strftime('%H:%M')

# Tách cột 'End Time' thành ngày và thời gian
df['End Date'] = pd.to_datetime(df['End Time']).dt.date
df['End Hour:Minute'] = pd.to_datetime(df['End Time']).dt.strftime('%H:%M')

# Sắp xếp lại thứ tự các cột
df = df[['Start Date', 'Start Hour:Minute', 'End Date', 'End Hour:Minute']]

# Gộp dữ liệu cùng ngày
df_grouped = df.groupby('Start Date').agg({
    'Start Hour:Minute': lambda x: ', '.join(x),
    'End Date': 'first',
    'End Hour:Minute': lambda x: ', '.join(x)
}).reset_index()

# Lưu dữ liệu thành tệp CSV
df_grouped.to_csv('time_intervals_grouped.csv', index=False)

print("Dữ liệu đã được gộp và lưu thành tệp CSV.")
