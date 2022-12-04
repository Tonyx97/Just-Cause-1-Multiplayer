using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace launcher.Helpers
{
    internal class BoolToServerProtectionConverter : IValueConverter
    {
        public BoolToServerProtectionConverter()
        {
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if(value is bool boolValue && boolValue)
            {
                return "Server is protected via password";
            }
            return "Server is not protected via password";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new ArgumentException("ExceptionBoolToServerProtectionConverterUnsupported");
        }
    }
}
