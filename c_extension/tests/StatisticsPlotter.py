import ast
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from typing import List

class StatisticsPlotter:
    """ handles all statistical analysis and visualization of experiment results"""

    REDUCE_PARAM_NAME = 'reduce_triggerSz_sizeLim'

    def __init__(self, resultFilepath: str, seed: str):
        self.resultFilepath = resultFilepath
        self.master_seed = seed
    

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
                
        return combined

    def plot_experiment_group(self, group_csv_results: list, independent_variable: str) -> None:
        df = pd.read_csv(group_csv_results)
        
        self.plot_timeNaccuracy_vs_iv(df, independent_variable)
        self.run_reduce_plot_suite(df, independent_variable)
    
    def run_reduce_plot_suite(self, df: pd.DataFrame, independent_variable: str) -> None:
        ''' check that IV is actually reduce, and then create relevant plots '''

        if independent_variable != self.REDUCE_PARAM_NAME:
            return
        
        self.plot_reduction_heatmap()
    
    def plot_timeNaccuracy_vs_iv(self, df: pd.DataFrame, indep_variable: str) -> str:
        """generate single plot with all three N vs time metrics"""
    
        # x values
        try:
            iv = df[indep_variable]
        except Exception as e:
            return f"error trying to get x-axis:  {e}"
        if pd.api.types.is_numeric_dtype(iv):
            x_pos = iv.values
            xtick_labels = iv.values
        else:
            x_pos = range(len(iv))
            xtick_labels = iv.values

        # y values
        min_mean_time = df['min_time_mean']
        max_mean_time = df['max_time_mean']
        sum_mean_time = df['sum_time_mean']
        
        # plots
        fig, ax = plt.subplots(2, 1, figsize=(12, 15))
        
        # TIME
        ax1 = ax[0]
        ax1.errorbar(iv, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple', label='MIN')
        ax1.errorbar(iv, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange', label='MAX')
        ax1.errorbar(iv, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green', label='SUM')

        # titles and labels
        ax1.set_title(f"Query Performance vs {indep_variable} (n={int(df['dataset_size'].mean())})", fontsize=14, fontweight='bold')
        ax1.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)
        
        step = max(1, len(x_pos) // 10)
        ax1.set_xticks(x_pos[::step])
        ax1.set_xticklabels(xtick_labels[::step], rotation=45, ha='right')

        # ACCURACY
        y = df['result_coverage_mean']
        ax2 = ax[1]
        ax2.errorbar(iv, y, marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple', label='MIN')
        ax2.errorbar(iv, y, marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange', label='MAX')
        ax2.errorbar(iv, y, marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green', label='SUM')

        # titles and labels
        ax2.set_title(f"Coverage vs {indep_variable} (n={int(df['dataset_size'].mean())})", fontsize=14, fontweight='bold')
        ax2.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax2.set_ylabel('result_coverage_mean (ms)', fontsize=12)
        ax2.legend(fontsize=11)
        ax2.grid(True, alpha=0.3)
        
        ax2.set_xticks(x_pos[::step])
        ax2.set_xticklabels(xtick_labels[::step], rotation=45, ha='right')

        plt.tight_layout()
        outfile = f'time_accuracy_sd{self.master_seed}'
        outpath = f"{self.resultFilepath}/{outfile}"
        plt.savefig(outpath)
        return outpath
    
    def plot_reduction_heatmap(self, df: pd.DataFrame, indep_variable: str) -> str:
        """generate heatmap for reduction parameter tuning"""
        
        if indep_variable != self.REDUCE_PARAM_NAME:
            return
        
        # parse tuple column
        parsed = df[self.REDUCE_PARAM_NAME].apply(
            lambda x: eval(x) if isinstance(x, str) else x
        )
        df['trigger_sz'] = parsed.apply(lambda x: x[0])
        df['reduce_to_sz'] = parsed.apply(lambda x: x[1])
        
        # pivot table input for heatmap 
        sum_pivot = df.pivot_table(values='sum_time_mean', index='reduce_to_sz', columns='trigger_sz')   
        
        fig, (ax) = plt.subplots(1, 1, figsize=(12, 5))

        # SUM heatmap, focused on reduction params
        sns.heatmap(sum_pivot, annot=True, fmt='.1f', cmap='RdYlGn_r', ax=ax, cbar_kws={'label': 'Time (ms)'})
        ax.set_title('SUM Time Heatmap', fontsize=14, fontweight='bold')
        ax.set_xlabel('Trigger Size', fontsize=12)
        ax.set_ylabel('Reduce To Size', fontsize=12)
        
        plt.tight_layout()
        outfile = f'heatmap{self.master_seed}'
        outpath = f"{self.resultFilepath}/{outfile}"
        plt.savefig(outpath, dpi=300, bbox_inches='tight')
        return outpath
    
    # def plot_pareto_front(self, df: pd.DataFrame)