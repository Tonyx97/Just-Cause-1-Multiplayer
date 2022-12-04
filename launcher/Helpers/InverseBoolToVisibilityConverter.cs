using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows;

namespace launcher.Helpers
{
    internal class InverseBoolToVisibilityConverter : IValueConverter
    {
        public InverseBoolToVisibilityConverter()
        {
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool boolValue)
            {
                if (!boolValue)
                {
                    return Visibility.Visible;
                }
                else
                {
                    if (parameter is string stringParameter && stringParameter == "Collapsed")
                    {
                        return Visibility.Collapsed;
                    }
                }
            }
            return Visibility.Hidden;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new ArgumentException("ExceptionInverseBoolToVisibilityConverterUnsupported");
        }
    }
}
