/*
** USAGE: debug: dotnet run test_files/example.txt
** COMPILE: dotnet publish -c Release --self-contained -r linux-x64 --framework netcoreapp2.2 -o csharp_binary
** RUN: ./csharp_binary/currency_converter ../test_files/example.txt or cp ./bin/Release/netcoreapp2.2/linux-x64/publish/currency_converter ./currency_converter
** WINDOWS COMPILE: dotnet publish -c Release -r win10-x64-x64 && cp bin/Release/netcoreapp2.2/win10-x64/publish/currency_converter.exe ./currency_converter.exe
*/

using System;
using System.IO;
using System.Threading;
using System.Collections.Generic;
using System.Linq;

namespace test_backend
{
    class CurrencyNode
    {
        public bool Visited {get;set;}
        public int Distance {get;set;} // distance from initial_ccy
        public int CurrencyID {get;set;}
        public decimal Exchange_rate {get;set;}

        public CurrencyNode(int currencyID, decimal exchange_rate) {
          CurrencyID = currencyID;
          Exchange_rate = exchange_rate;
        }

        public CurrencyNode() {
          Visited = false;
          Distance = Int32.MaxValue;
          CurrencyID = -1;
          Exchange_rate = 1;
        }

        public void SetVisited(int distance, int currencyID, decimal exchange_rate) {
            Visited = true;
            Distance = distance + 1;
            CurrencyID = currencyID;
            Exchange_rate = exchange_rate;
        }
    }

    class Graph
    {
        static private HashSet<int> different_currencies = new HashSet<int>(); // each different currency ticker got from the input file
        static private int v; // used to get HashSet nb of items once is as been built
        static private Dictionary<int, int> ccy_id = new Dictionary<int, int>(); // map the hash code of each CCY to an ID starting at index 0
        static private List<CurrencyNode>[] vertex; // stores all the exchange rates in both direction. i.e : if EUR to KRW is 1300 then KRW to EUR is 1/1300

        static private int initial_ccy;
        static private int desired_ccy;
        static private decimal notional;

        // to have a unique number for each ticker
        static int make_guid(string ticker) {
            if (ticker.Length != 3) {
                Console.Error.WriteLine($@"Error: Ticker {ticker} should have exactly 3 characters");
                Environment.Exit(1);
            }
            return ((ticker[0] << 16) | (ticker[1] << 8) | ticker[2]);
        }

        /*
        ** graph creation
        */

        public void create_graph(List<string> lines)
        {
            List<Tuple<int,int,decimal>> ccy_pairs = get_ccy_pairs(lines);
            set_ccy_ids();
            get_currencies_to_convert(lines);
            build_graph(ccy_pairs);
        }

        private List<Tuple<int,int,decimal>> get_ccy_pairs(List<string> lines)
        {
            var ccy_pairs = new List<Tuple<int,int,decimal>>();

            List<string> tickers = new List<string>(lines[0].Split(";"));

            different_currencies.Add(make_guid(tickers[0]));
            different_currencies.Add(make_guid(tickers[2]));

            for (int i = 2; i < lines.Count; i++) {
                tickers = new List<string>(lines[i].Split(";"));

                if (tickers.Count != 3) {
                    Console.Error.WriteLine($@"Error: Wrong Format File on line {i}: Each line should countain 'Ticker;Ticker;ExchangeRate'");
            		Environment.Exit(1);
                }

                int from_ccy = make_guid(tickers[0]);
                different_currencies.Add(from_ccy);
                int to_ccy = make_guid(tickers[1]);
                different_currencies.Add(to_ccy);
                decimal exchange_rate = 0;

                try {
                    exchange_rate = decimal.Parse(tickers[2]);
                }
                catch (System.FormatException e){
                    Console.Error.WriteLine($"Error: Line {i}, From third line the third argument (with ';' as separator) should be a decimal {e}");
                    Environment.Exit(1);
                }

                if (exchange_rate <= 0) {
                    Console.Error.WriteLine($"Error: Line {i}, exchange rate {exchange_rate} should not be negative or equal to 0");
                    Environment.Exit(1);
                }
                ccy_pairs.Add(Tuple.Create(from_ccy, to_ccy, exchange_rate));
            }

            return ccy_pairs;
        }

        private void set_ccy_ids()
        {
            // assign id 0, 1, 2 etc to each different ccy
            int id = 0;
            foreach (int ccy in different_currencies) {
                ccy_id[ccy] = id++;
            }
            v = id;// above variable that specifies the number of different currencies

            vertex = new List<CurrencyNode>[v];
            for (int i = 0; i < id; i++)
                vertex[i] = new List<CurrencyNode>();
        }

        private void get_currencies_to_convert(List<string> lines)
        {
            List<string> tickers = new List<string>(lines[0].Split(";"));

            initial_ccy = ccy_id[make_guid(tickers[0])];
            desired_ccy = ccy_id[make_guid(tickers[2])];
            notional = decimal.Parse(tickers[1]);

            #if DEBUG
                Console.WriteLine($"You want to convert {notional} {tickers[0]}-{initial_ccy} to {tickers[2]}-{desired_ccy}");
            #endif
        }

        private void build_graph(List<Tuple<int,int,decimal>> ccy_pairs)
        {
            foreach (var ccy_pair in ccy_pairs) {
                int from_ccy_id = ccy_id[ccy_pair.Item1];
                int to_ccy_id = ccy_id[ccy_pair.Item2];
                decimal xch_rate = ccy_pair.Item3;

                if (xch_rate == 0) {
                    Console.Error.WriteLine($@"Error: exchange rate cannot be equal to 0");
            		Environment.Exit(1);
                }
                // add vertex to the graph
                vertex[from_ccy_id].Add(new CurrencyNode(to_ccy_id, xch_rate));
                vertex[to_ccy_id].Add(new CurrencyNode(from_ccy_id, 1 / xch_rate));
            }
        }

        /*
        ** solve functions
        */

        public void solve_graph()
        {
            CurrencyNode[] currencyNode = new CurrencyNode[v];

            if (BFS(currencyNode) == false)
            {
                Console.Error.WriteLine("Impossible to convert as missing exchange rate pair(s)");
                Environment.Exit(1);
            }

            List<CurrencyNode> path = new List<CurrencyNode>(get_shortest_path(currencyNode));

            for (int i = path.Count - 1; i >= 0; i--) {
                notional *= decimal.Round(path[i].Exchange_rate, 4);
            }

            Console.WriteLine(decimal.Round(notional));
        }

        private bool BFS(CurrencyNode[] currencyNode)
        {
            LinkedList<int> queue = new LinkedList<int>();
            for (int i = 0; i < v; i++) {
                currencyNode[i] = new CurrencyNode();
            }

            currencyNode[initial_ccy].Visited = true;
            currencyNode[initial_ccy].Distance = 0;
            queue.AddLast(initial_ccy);

            while (queue.Count != 0) {
                int u = queue.First();
                queue.RemoveFirst();
                for (int i = 0; i < vertex[u].Count; i++) {
                    int current_ccy = vertex[u][i].CurrencyID;

                    if (currencyNode[current_ccy].Visited == false) {
                        currencyNode[current_ccy].SetVisited(currencyNode[u].Distance, u, vertex[u][i].Exchange_rate);
                        queue.AddLast(current_ccy);

                        if (current_ccy == desired_ccy) {
                            return true; // We stop BFS when we reach the desired currency.
                        }
                    }
                }
            }
            return false;
        }

        private static List<CurrencyNode> get_shortest_path(CurrencyNode[] currencyNode)
        {
            List<CurrencyNode> path = new List<CurrencyNode>();
            int i = desired_ccy;

            path.Add(new CurrencyNode(i, 1));

            while (currencyNode[i].CurrencyID != -1) {
                path.Add(new CurrencyNode(currencyNode[i].CurrencyID, currencyNode[i].Exchange_rate));
                i = currencyNode[i].CurrencyID;
            }
            return path;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args == null || args.Length == 0) {
                display_usage();
            }

            for (int i = 0; i < args.Length; i++) {
                String filename = args[i];

                List<string> lines = new List<string>(parse_file(filename)); // each lines of the file
                check_number_of_ccy_pairs(lines); // number of expected lines of currency pairs

                Graph graph = new Graph();
                graph.create_graph(lines);
                graph.solve_graph(); // shortest path
            }
        }

        static void display_usage()
        {
            Console.Error.WriteLine(@"Please provide a file as input
            File should be formatted the following way:
            EUR;550;JPY\n6\nAUD;CHF;0.9661\nJPY;KRW;13.1151\nEUR;CHF;1.2053
            AUD;JPY;86.0305\nEUR;USD;1.2989\nJPY;INR;0.6571
            First line Means that you want to convert 550 EUR to JPY
            Second line means that you will provide 6 unique exchange rates
            Following lines are the exchange rate, for example 4th line means that you can exchange 1 JPY for 13.1151 KRW\n");
        }

        static List<string> parse_file(String filename)
        {
            FileStream fileStream = new FileStream(filename, FileMode.Open);
            List<string> lines = new List<string>();

            using (StreamReader reader = new StreamReader(fileStream))
            {
                String line;
                while ((line = reader.ReadLine()) != null) {
                    lines.Add(line);
                }
            }
            return lines;
        }

        static int check_number_of_ccy_pairs(List<string> lines)
        {
            if (lines.Count < 3)
            {
                Console.Error.WriteLine("Too small file");
                display_usage();
                Environment.Exit(1);
            }


            try {
                int expected_ccy_pairs = Int32.Parse(lines[1]);

                if (expected_ccy_pairs < 0)
                {
                    Console.Error.WriteLine($"Second line of the file '{expected_ccy_pairs}' should be a positive number indicating the number of ccy pairs.");
                    Environment.Exit(1);
                }
                if (expected_ccy_pairs != lines.Count - 2)
                {
                    Console.Error.WriteLine($@"Number of expected pairs does not match provided ccy pairs:
                    Expected ccy pairs: {expected_ccy_pairs}
                    Actual ccy pairs: {lines.Count - 2}");
            		Environment.Exit(1);
                }

                return expected_ccy_pairs;
            }
            catch (System.FormatException e){
                Console.Error.WriteLine($"Second line of the file should be an integer {e}");
                Environment.Exit(1);
            }
            return 0;
        }
    }
}
