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
    /// Hierarchy.xaml 的交互逻辑
    /// </summary>
    public partial class Hierarchy : Page
    {
        public Hierarchy()
        {
            InitializeComponent();
        }

        private void TabItem_MouseMove_1(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                TabItem t = e.Source as TabItem;
                if (t == null) return;
                DataObject dataObject = new DataObject(typeof(TabItem), sender);
                DragDrop.DoDragDrop(t, dataObject, DragDropEffects.Move);
            }
        }
    }
}
