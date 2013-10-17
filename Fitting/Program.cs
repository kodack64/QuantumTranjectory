using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace FittingPoisson
{
    class Program
    {
        private Dictionary<int, double> map = new Dictionary<int, double>();
        private List<int> keyList = new List<int>();
        private double minDif = 1000;
        private double minMean = 0;

        public virtual void fileRead(string file)
        {
            try
            {
                StreamReader sr = new StreamReader(file);
                string line;
                double sum = 0;
                while ((line = sr.ReadLine()) != null)
                {
                    string[] array = line.Split(' ');
                    int id = int.Parse(array[0]);
                    double num = double.Parse(array[1]);
                    map.Add(id,num);
                    keyList.Add(id);
                    sum += num;
                }
                foreach (int entry in keyList)
                {
                    map[entry] /= sum;
                }

                foreach (KeyValuePair<int, double> entry in map)
                {
                    Console.Out.WriteLine("entry is " + entry.Key.ToString() + "=" + entry.Value.ToString());
                }
                sr.Close();
            }
            catch (Exception e)
            {
                Console.Out.WriteLine(e.Message);
            }
        }
        public virtual double poisson(double mean, int eventNum)
        {
            double val = Math.Pow(mean,eventNum)*Math.Exp(-mean);
            while (eventNum > 1)
            {
                val /= eventNum;
                eventNum--;
            }
            return val;
        }
        public virtual void optimize()
        {
            double sum = 0;

            double guessMean=0;

            for (guessMean = 0; guessMean < 10.0; guessMean += 0.0001)
            {
                sum = 0;
                foreach (KeyValuePair<int, double> entry in map)
                {
                    double val = poisson(guessMean, entry.Key);
                    sum += Math.Pow(val - entry.Value, 2);
                }
                if (minDif > sum)
                {
                    minDif = sum;
                    minMean = guessMean;
                }
            }
            Console.Out.WriteLine(String.Format("{0,-7}", minMean.ToString("0.0000")) + " " + minDif);
        }
        public virtual void fileWrite(string file)
        {
            try
            {
                StreamWriter sw = new StreamWriter(file);
                sw.WriteLine("#mean="+minMean.ToString()+" dif="+minDif.ToString());
                foreach (int key in keyList)
                {
                    sw.WriteLine(key.ToString()+" "+map[key].ToString()+" "+poisson(minMean,key).ToString());
                }
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
            prog.fileRead("hit.txt");
            prog.optimize();
            prog.fileWrite("fit.txt");
        }
    }
}
