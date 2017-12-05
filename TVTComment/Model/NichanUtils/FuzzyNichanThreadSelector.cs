﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace TVTComment.Model.NichanUtils
{
    class FuzzyNichanThreadSelector:INichanThreadSelector
    {
        private static int getLevenshteinDistance(string str1,string str2)
        {
            int[,] d = new int[str1.Length+1, str2.Length+1];
            for (int i = 0; i <= str1.Length; i++)
                d[i, 0] = i;
            for (int i = 0; i <= str2.Length; i++)
                d[0, i] = i;

            for(int i=0;i<str1.Length;i++)
                for(int j=0;j<str2.Length;j++)
                {
                    int cost = str1[i] == str2[j] ? 0 : 1;
                    d[i+1, j+1] = new int[3] { d[i , j+1] + 1, d[i+1, j] + 1, d[i, j] + cost }.Min();
                }
            return d[str1.Length, str2.Length];
        }

        private static readonly Regex reBracket = new Regex(@"(【.*】)");
        private static string normalizeThreadTitle(string title)
        {
            title=reBracket.Replace(title, "");
            title=title.Replace("無断転載禁止", "");
            title=title.Replace("©", "");
            title=title.Replace("2ch.net", "");
            title=title.Replace("&#169;", "");
            return title;
        }

        public Uri BoardUri { get; }
        public string ThreadTitleExample { get; }
        public FuzzyNichanThreadSelector(Uri boardUri,string threadTitleExample)
        {
            BoardUri = boardUri;
            ThreadTitleExample = normalizeThreadTitle(threadTitleExample);
        }

        public IEnumerable<string> Get(ChannelInfo channel,DateTime time)
        {
            Nichan.Board board = Nichan.BoardParser.ParseFromUri(BoardUri.ToString());

            var threads = board.Threads.Where(x => x.ResCount <= 1000).
                Select(x => new { Thread = x, EditDistance = getLevenshteinDistance(normalizeThreadTitle(x.Title), ThreadTitleExample) }).
                OrderBy(x => x.EditDistance);

            if (threads.FirstOrDefault() == null) return new string[0];

            //レーベンシュタイン距離の最小値から距離10以内のものを選択
            return threads.TakeWhile(x => x.EditDistance <= threads.First().EditDistance+10).Take(3).Select(x => x.Thread.Uri.ToString());
        }
    }
}
