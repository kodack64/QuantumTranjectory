using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Histgram
{
    class Program
    {
        private Dictionary<int, double> map = new Dictionary<int, double>();
        private List<int> keyList = new List<int>();
        private List<List<double>> timeList = new List<List<double>>();
        private double rangeMax;
        private List<int[]> histgramList = new List<int[]>();
        private int maxCount;
        private int rangeHistgram;
        private double div;

        public virtual void fileRead(string filehead)
        {
            rangeMax = 0;
            int hi;
            for (hi = 1; ; hi++)
            {
                try
                {
                    string file = filehead + hi.ToString() + ".txt";
                    if (!File.Exists(file))
                    {
                        break;
                    }
                    StreamReader sr = new StreamReader(file);
                    string line;
                    List<double> temp = new List<double>();
                    while ((line = sr.ReadLine()) != null)
                    {
                        string[] array = line.Split(' ');
                        if (array.Length < hi)
                        {
                            continue;
                        }

                        for (int ite = 0; ite < hi; ite++)
                        {
                            double val = double.Parse(array[ite]);
                            temp.Add(val);
                            if (val > rangeMax) rangeMax = val;
                        }
                    }
                    timeList.Add(temp);
                    sr.Close();
                }
                catch (Exception e)
                {
                    Console.Out.WriteLine(e.Message);
                }
            }
        }
        public virtual void divide(double _div)
        {
            div = _div;
            maxCount = timeList.Count();
            rangeHistgram = (int)(rangeMax / div)+2;

            int[] histgram;

            histgram = new int[rangeHistgram];
            for (int x = 0; x < histgram.Length; x++) histgram[x] = 0;
            for (int hi = 0; hi < maxCount; hi++)
            {
                for (int id = 0; id < timeList[hi].Count(); id++)
                {
                    histgram[(int)(timeList[hi][id] / div)]++;
                }
            }
            histgramList.Add(histgram);

            for (int hi = 0; hi < maxCount; hi++)
            {
                histgram = new int[rangeHistgram];
                for (int x = 0; x < histgram.Length; x++) histgram[x] = 0;
                for (int id = 0; id < timeList[hi].Count(); id++)
                {
                    histgram[(int)(timeList[hi][id] / div)]++;
                }
                histgramList.Add(histgram);
            }
        }
        public virtual void fileWrite(string file)
        {
            int maxHistData = 0;
            double width = 1;
            try
            {
                if (File.Exists("property.txt"))
                {
                    StreamReader sr = new StreamReader("property.txt");
                    string line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        string[] array = line.Split('=');
                        if (array.Count() >= 2)
                        {
                            if (array[0].Trim() == "width")
                            {
                                width = double.Parse(array[1]);
                                break;
                            }
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Console.Out.WriteLine(e.Message);
            }
            try
            {
                StreamWriter sw = new StreamWriter(file);
                for (int i = 0; i < rangeHistgram; i++)
                {
                    sw.Write((div * i).ToString() + " ");
                    for (int j = 0; j < histgramList.Count(); j++)
                    {
                        sw.Write(histgramList[j][i].ToString()+" ");
                        if (histgramList[j][i] > maxHistData)
                        {
                            maxHistData = histgramList[j][i];
                        }
                    }
                    sw.WriteLine();
                }
                sw.Close();
                sw = new StreamWriter("plot.txt");
                sw.WriteLine("set xlabel \"time(micro sec)\"");
                sw.WriteLine("set ylabel \"count\"");
                sw.WriteLine("plot x<+"+width.ToString()+"?(-cos(x/"+width.ToString()+"*2*3.14159)+1)*"+maxHistData.ToString()+"/2:0 ti \"input\"");
                sw.Write("replot \"hist.txt\" u 1:2 w l ti \"all\"");
                for (int i = 0; i < maxCount; i++)
                {
                    sw.Write(" ,\"\" u 1:"+(i+3).ToString()+" w l ti \""+(i+1).ToString()+"photon\"");
                }
                sw.WriteLine();
                sw.WriteLine("pause -1");
                sw.Close();
            }
            catch (Exception e)
            {
                Console.Out.WriteLine(e.Message);
            }
        }
        static void Main(string[] args)
        {
            Program prog = new Program();
            prog.fileRead("hit");
            double div;
            while (true)
            {
                Console.Out.Write(">");
                string str = Console.ReadLine();
                if (double.TryParse(str,out div)) break;
            }
            prog.divide(div);
            prog.fileWrite("hist.txt");
        }
    }
}
