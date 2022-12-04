using System;
using System.Collections.Generic;

namespace launcher.Helpers.Extensions
{
    public static class ArrayExtensions
    {
        public static T[] CopySlice<T>(this T[] source, int index, int length, bool pad_to_length = false)
        {
            int n = length;
            T[]? slice = null;

            if (source.Length < index + length)
            {
                n = source.Length - index;
                if (pad_to_length)
                {
                    slice = new T[length];
                }
            }

            slice ??= new T[n];
            Array.Copy(source, index, slice, 0, n);
            return slice;
        }

        public static IEnumerable<T[]> Slices<T>(this T[] source, int count, bool pad_to_length = false)
        {
            for (var i = 0; i < source.Length; i += count)
                yield return source.CopySlice(i, count, pad_to_length);
        }
    }
}
