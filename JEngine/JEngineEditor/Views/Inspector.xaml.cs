using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace JEngineEditor.Views
{
    /// <summary>
    /// Inspector.xaml 的交互逻辑
    /// </summary>
    public partial class Inspector : Page
    {
        public Inspector()
        {
            InitializeComponent();
        }

        private void TabItem_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(TabItem)))
            {
                TabItem t = e.Data.GetData(typeof(TabItem)) as TabItem;
                if (t == sender) return;
                if (t == null) return;
                TabControl p = t.Parent as TabControl;
                if (p == null) return;
                p.Items.Remove(t);
                tc.Items.Add(t);
            }
        }
        private TabItem GetTargetTabItem(object originalSource)
        {
            var current = originalSource as DependencyObject;

            while (current != null)
            {
                var tabItem = current as TabItem;
                if (tabItem != null)
                {
                    return tabItem;
                }

                current = VisualTreeHelper.GetParent(current);
            }

            return null;
        }

        private void TabItem_DragOver(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(typeof(TabItem)))
            {
                e.Effects = DragDropEffects.None;
                e.Handled = true;
            }
        }
    }
}
