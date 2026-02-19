import ast
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from typing import List

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


    def parse_reduce_tuple(self, val):
        """Parse reduce_triggerSz_sizeLim column."""
        try:
            return ast.literal_eval(str(val))
        except:
            return None

    def load_all_csvs(self, csv_paths: List[str]) -> pd.DataFrame:
        """Load and combine multiple experiment CSVs."""
        dfs = []
        for path in csv_paths:
            df = pd.read_csv(path, index_col=0)
            df['source_file'] = Path(path).parent.name  # track which experiment it came from
            dfs.append(df)
        
        combined = pd.concat(dfs, ignore_index=True)
        
        # Parse reduce params
        if 'reduce_triggerSz_sizeLim' in combined.columns:
            parsed = combined['reduce_triggerSz_sizeLim'].apply(self.parse_reduce_tuple)
            combined['trigger_sz'] = parsed.apply(lambda x: x[0] if x else None)
            combined['size_lim']   = parsed.apply(lambda x: x[1] if x else None)
        
        return combined

    def generate_3_agg_plots(self, csv_path: str, indep_variable: str):
        """generate 3-panel line plots for MIN, MAX, SUM"""
        
        experiment_folder_path = self.resultFilepath
        agg_out_file = f'aggregate_results_sd{self.master_seed}'
        agg_results_path =  f'{experiment_folder_path}/{agg_out_file}.jpg'

        df = pd.read_csv(csv_path)
        fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(14, 5))
        
        # x values
        try:
            n = df[indep_variable]
        except Exception as e:
            return f"error trying to get x-axis:  {e}"
        if pd.api.types.is_numeric_dtype(n):
            x_pos = n.values
            xtick_labels = n.values
        else:
            x_pos = range(len(n))
            xtick_labels = n.values
        step = max(1, len(x_pos) // 10)
        
        # y-values
        min_mean_time = df['min_time_mean']
        max_mean_time = df['max_time_mean']
        sum_mean_time = df['sum_time_mean']

        # MIN
        ax1.errorbar(n, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple')
        ax1.set_title("Mean Time of MIN", fontsize=14, fontweight='bold')
        ax1.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax1.set_xticks(x_pos[::step])
        ax1.set_xticklabels(xtick_labels[::step], rotation=45, ha='right')
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.grid(True, alpha=0.3)

        # MAX
        ax2.errorbar(n, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange')
        ax2.set_title("Mean Time of MAX", fontsize=14, fontweight='bold')
        ax2.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax2.set_xticks(x_pos[::step])
        ax2.set_xticklabels(xtick_labels[::step], rotation=45, ha='right')
        ax2.set_ylabel('Time (ms)', fontsize=12)
        ax2.grid(True, alpha=0.3)

        # SUM
        ax3.errorbar(n, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green')
        ax3.set_title("Mean Time of SUM", fontsize=14, fontweight='bold')
        ax3.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax3.set_xticks(range(len(n)))
        ax3.set_xticks(x_pos[::step])
        ax3.set_xticklabels(xtick_labels[::step], rotation=45, ha='right')
        ax3.set_ylabel('Time (ms)', fontsize=12)
        ax3.grid(True, alpha=0.3)

        plt.tight_layout()
        plt.plot()
        plt.savefig(agg_results_path)

        return agg_results_path

    def generate_combined_agg_plot(self, csv_path: str, indep_variable):
        """generate single plot with all three metrics"""
        experiment_folder_path = self.resultFilepath
        combined_results_path = f'combined_agg_results_sd{self.master_seed}'
        combined_results_path = f'{experiment_folder_path}/{combined_results_path}.jpg'

        df = pd.read_csv(csv_path)
        
        # x values
        try:
            n = df[indep_variable]
        except Exception as e:
            return f"error trying to get x-axis:  {e}"
        if pd.api.types.is_numeric_dtype(n):
            x_pos = n.values
            xtick_labels = n.values
        else:
            x_pos = range(len(n))
            xtick_labels = n.values

        # y values
        min_mean_time = df['min_time_mean']
        max_mean_time = df['max_time_mean']
        sum_mean_time = df['sum_time_mean']
        
        fig, ax = plt.subplots(figsize=(12, 5))

        # plots
        ax.errorbar(n, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple', label='MIN')
        ax.errorbar(n, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange', label='MAX')
        ax.errorbar(n, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green', label='SUM')

        # titles and labels
        ax.set_title(f"Query Performance vs {indep_variable})", fontsize=14, fontweight='bold')
        ax.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax.set_ylabel('Time (ms)', fontsize=12)
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        
        step = max(1, len(x_pos) // 10)
        ax.set_xticks(x_pos[::step])
        ax.set_xticklabels(xtick_labels[::step], rotation=45, ha='right')

        plt.tight_layout()
        plt.savefig(combined_results_path)
        # plt.plot()

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
        sum_pivot = df.pivot_table(values='sum_time_mean', index='reduce_to_sz', columns='trigger_sz')   
        
        fig, (ax) = plt.subplots(1, 1, figsize=(12, 5))

        # SUM heatmap
        sns.heatmap(sum_pivot, annot=True, fmt='.1f', cmap='RdYlGn_r', ax=ax, cbar_kws={'label': 'Time (ms)'})
        ax.set_title('SUM Time Heatmap', fontsize=14, fontweight='bold')
        ax.set_xlabel('Trigger Size', fontsize=12)
        ax.set_ylabel('Reduce To Size', fontsize=12)
        
        plt.tight_layout()
        plt.savefig(f'{csv_path}_heatmap.jpg', dpi=300, bbox_inches='tight')
        
        # return f'{csv_path}_heatmap.jpg'


    def generate_reduction_heatmap2(self, df: pd.DataFrame, csv_path: str, indep_variable: str):
        """generate heatmap for reduction parameter tuning"""
        
        if indep_variable != "reduce_triggerSz_sizeLim":
            return
        
        # parse tuple column
        parsed = df['reduce_triggerSz_sizeLim'].apply(
            lambda x: eval(x) if isinstance(x, str) else x
        )
        df['trigger_sz'] = parsed.apply(lambda x: x[0])
        df['reduce_to_sz'] = parsed.apply(lambda x: x[1])
        
        # pivot table input for heatmap 
        sum_pivot = df.pivot_table(values='sum_time_mean', index='reduce_to_sz', columns='trigger_sz')   
        
        fig, (ax) = plt.subplots(1, 1, figsize=(12, 5))

        # SUM heatmap
        sns.heatmap(sum_pivot, annot=True, fmt='.1f', cmap='RdYlGn_r', ax=ax, cbar_kws={'label': 'Time (ms)'})
        ax.set_title('SUM Time Heatmap', fontsize=14, fontweight='bold')
        ax.set_xlabel('Trigger Size', fontsize=12)
        ax.set_ylabel('Reduce To Size', fontsize=12)
        
        plt.tight_layout()
        plt.savefig(f'{csv_path}_heatmap.jpg', dpi=300, bbox_inches='tight')
        
        # return f'{csv_path}_heatmap.jpg'
