import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Optional

class StatisticsPlotter:
    """ handles all statistical analysis and visualization of experiment results"""

    def __init__(self, resultFilepath: str, seed: str):
        self.resultFilepath = resultFilepath
        self.master_seed = seed
    
    def plot_all(self, csv_path: str, indep_variable: str):
        """ calls all plotting functions """

        three_agg_path = self.generate_3_agg_plots(csv_path, indep_variable)
        comb_agg_path = self.generate_combined_agg_plot(csv_path, indep_variable)
        heat_agg_path = self.generate_reduction_heatmap(csv_path, indep_variable)
        
        print(f"  3 aggs saved: {three_agg_path}")
        print(f"  Combined aggs saved: {comb_agg_path}")
        if heat_agg_path: 
            print(f"  Heatmap saved: {heat_agg_path}")


    def generate_3_agg_plots(self, csv_path: str, indep_variable: str):
        """generate 3-panel line plots for MIN, MAX, SUM"""
        
        experiment_folder_path = self.resultFilepath
        agg_out_file = f'aggregate_results_sd{self.master_seed}'
        agg_results_path =  f'{experiment_folder_path}/{agg_out_file}.jpg'

        df = pd.read_csv(csv_path)
        fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(14, 5))
        
        # get the variable of the experiment #FIXME add in multi variable version & diff plots perhaps
        try:
            n = df[indep_variable]
        except Exception as e:
            return f"error trying to get x-axis:  {e}"
            
        min_mean_time = df['min_time_mean']
        max_mean_time = df['max_time_mean']
        sum_mean_time = df['sum_time_mean']

        # MIN
        ax1.errorbar(n, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple')
        ax1.set_title("Mean Time of MIN", fontsize=14, fontweight='bold')
        ax1.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax1.set_xticks(range(len(n)))
        ax1.set_xticklabels(n, rotation=45, ha='right')
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.grid(True, alpha=0.3)

        # MAX
        ax2.errorbar(n, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange')
        ax2.set_title("Mean Time of MAX", fontsize=14, fontweight='bold')
        ax2.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax2.set_xticks(range(len(n)))
        ax2.set_xticklabels(n, rotation=45, ha='right')
        ax2.set_ylabel('Time (ms)', fontsize=12)
        ax2.grid(True, alpha=0.3)

        # SUM
        ax3.errorbar(n, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green')
        ax3.set_title("Mean Time of SUM", fontsize=14, fontweight='bold')
        ax3.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax3.set_xticks(range(len(n)))
        ax3.set_xticklabels(n, rotation=45, ha='right')
        ax3.set_ylabel('Time (ms)', fontsize=12)
        ax3.grid(True, alpha=0.3)

        plt.tight_layout()
        plt.savefig(agg_results_path)

        return agg_results_path


    def generate_combined_agg_plot(self, csv_path: str, indep_variable):
        """generate single plot with all three metrics"""
        experiment_folder_path = self.resultFilepath
        combined_results_path = f'combined_agg_results_sd{self.master_seed}'
        combined_results_path = f'{experiment_folder_path}/{combined_results_path}.jpg'

        df = pd.read_csv(csv_path)
        
        # get the variable of the experiment #FIXME add in multi variable version & diff plots perhaps
        try:
            n = df[indep_variable]
        except Exception as e:
            return f"error trying to get x-axis:  {e}"
            
        min_mean_time = df['min_time_mean']
        max_mean_time = df['max_time_mean']
        sum_mean_time = df['sum_time_mean']
        
        fig, ax = plt.subplots(figsize=(12, 5))

        ax.errorbar(n, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple', label='MIN')
        ax.errorbar(n, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange', label='MAX')
        ax.errorbar(n, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green', label='SUM')

        ax.set_title(f"Query Performance vs {indep_variable})", fontsize=14, fontweight='bold')
        ax.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax.set_xticks(range(len(n)))
        ax.set_xticklabels(n, rotation=45, ha='right')
        ax.set_ylabel('Time (ms)', fontsize=12)
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        plt.tight_layout()

        plt.tight_layout()
        plt.savefig(combined_results_path)

        return combined_results_path

    def generate_reduction_heatmap(self, csv_path: str, indep_variable: str):
        """generate heatmap for reduction parameter tuning"""
        
        if indep_variable != "reduce_triggerSz_sizeLim":
            return

        df = pd.read_csv(csv_path)
        
        # parse tuple column
        parsed = df['reduce_triggerSz_sizeLim'].apply(
            lambda x: eval(x) if isinstance(x, str) else x
        )
        df['trigger_sz'] = parsed.apply(lambda x: x[0])
        df['reduce_to_sz'] = parsed.apply(lambda x: x[1])
        
        # pivot table input for heatmap 
        sum_pivot = df.pivot_table(values='sum_time_mean', 
                                    index='reduce_to_sz', 
                                    columns='trigger_sz')   
        
        fig, (ax) = plt.subplots(1, 1, figsize=(12, 5))

        # SUM heatmap
        sns.heatmap(sum_pivot, annot=True, fmt='.1f', cmap='RdYlGn_r', ax=ax, cbar_kws={'label': 'Time (ms)'})
        ax.set_title('SUM Time Heatmap', fontsize=14, fontweight='bold')
        ax.set_xlabel('Trigger Size', fontsize=12)
        ax.set_ylabel('Reduce To Size', fontsize=12)
        
        plt.tight_layout()
        plt.savefig(f'{csv_path}_heatmap.jpg', dpi=300, bbox_inches='tight')
        
        return f'{csv_path}_heatmap.jpg'
    

if __name__ == '__main__':
    csv_path = "/Users/asxvi/Desktop/uic/research/audb/extension/c_extension/tests/data/results/reduction_param_tuning/d14_m02_y2026_s_n1K_unc00_ni7_gsr0_100_redSz50_45__iv_red_sz_sd1579190252/results_sd1579190252.csv"
    
    df = pd.read_csv(csv_path)

    print(df['independent_variable']) 
    
    pass