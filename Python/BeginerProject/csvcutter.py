import pandas as pd
import glob

def concatenate_csv(files, output_file):
    # List to hold the dataframes
    dataframes = []
    
    # Loop through the list of files and read each one into a dataframe
    for file in files:
        df = pd.read_csv(file)
        dataframes.append(df)
    
    # Concatenate all dataframes into a single dataframe
    concatenated_df = pd.concat(dataframes, ignore_index=True)
    
    # Save the concatenated dataframe to a new CSV file
    concatenated_df.to_csv(output_file, index=False)
    
    print(f'Files concatenated successfully into {output_file}')

# List of CSV files to concatenate
csv_files = ['river_level_data.csv','file2.csv', 'river_level_data1.csv']
output_csv = 'concatenated_output.csv'

# Call the function to concatenate the CSV files
concatenate_csv(csv_files, output_csv)
