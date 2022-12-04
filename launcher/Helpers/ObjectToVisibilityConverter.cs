using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace launcher.Helpers
{
    internal class ObjectToVisibilityConverter : IValueConverter
    {
        public ObjectToVisibilityConverter()
        {
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null)
            {
                return Visibility.Visible;
            }
            if(parameter is string stringParameter && stringParameter == "Collapsed")
            {
                return Visibility.Collapsed;
            }
            return Visibility.Hidden;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new ArgumentException("ExceptionObjectToVisibilityConverterUnsupported");
        }
    }
}
