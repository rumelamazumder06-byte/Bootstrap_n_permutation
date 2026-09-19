import pandas as pd
import matplotlib.pyplot as plt
try:
    df=pd.read_csv('shuffled_differences.csv')
except FileNotFoundError:
    print("The file 'shuffled_differences.csv' was not found. Please check the file path and try again.")
    exit()
plt.figure(figsize=(10,6))
plt.hist(df['Permuted_Difference'], bins=30, color='skyblue', edgecolor='black')
plt.title('Alpha vs Market Noice detection', fontsize=16, fontweight='bold')
plt.xlabel('Mean returns difference between groups ', fontsize=14)
plt.ylabel('Frequency', fontsize=14)
plt.grid(axis='y',linestyle='--', color='gray', alpha=0.75)
plt.legend()
print("custom trading statistics histo plot")
plt.show()
