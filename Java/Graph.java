import java.util.*;
import java.math.RoundingMode;
import java.math.BigDecimal;
import java.math.*;

public class Graph
{
    static private HashSet<Integer> different_currencies = new HashSet<Integer>(); // each different currency ticker got from the input file
    static private int v; // used to get HashSet nb of items once is as been built
    private HashMap<Integer, Integer> ccy_id = new HashMap<Integer, Integer>(); // map the hash code of each CCY to an ID starting at index 0
    static private ArrayList<CurrencyNode>[] vertex; // stores all the exchange rates in both direction. i.e : if EUR to KRW is 1300 then KRW to EUR is 1/1300

    static private int initial_ccy;
    static private int desired_ccy;
    static private java.math.BigDecimal notional;

    // to have a unique number for each ticker
    static int make_guid(String ticker) {
        if (ticker.length() != 3) {
            System.err.println("Error: Ticker {ticker} should have exactly 3 characters");
            System.exit(0);
        }
        return ((ticker.charAt(0) << 16) | (ticker.charAt(1) << 8) | ticker.charAt(2));
    }

    /*
    ** graph creation
    */

    public void create_graph(ArrayList<String> lines)
    {
        List<Tuple<Integer,Integer,java.math.BigDecimal>> ccy_pairs = get_ccy_pairs(lines);
        set_ccy_ids();
        get_currencies_to_convert(lines);
        build_graph(ccy_pairs);
    }

    private List<Tuple<Integer,Integer,java.math.BigDecimal>> get_ccy_pairs(ArrayList<String> lines)
    {
        var ccy_pairs = new ArrayList<Tuple<Integer,Integer,java.math.BigDecimal>>();

        var tickers = new ArrayList<String>(Arrays.asList(lines.get(0).split(";")));

        different_currencies.add(make_guid(tickers.get(0)));
        different_currencies.add(make_guid(tickers.get(2)));

        for (int i = 2; i < lines.size(); i++) {
            tickers = new ArrayList<String>(Arrays.asList(lines.get(i).split(";")));

            if (tickers.size() != 3) {
                System.err.println("Error: Wrong Format File on line " + i + ": Each line should countain 'Ticker;Ticker;ExchangeRate'");
                System.exit(0);
            }

            int from_ccy = make_guid(tickers.get(0));
            different_currencies.add(from_ccy);
            int to_ccy = make_guid(tickers.get(1));
            different_currencies.add(to_ccy);
            java.math.BigDecimal exchange_rate = new java.math.BigDecimal(0);

            try {
                exchange_rate = new java.math.BigDecimal(tickers.get(2));
            }
            catch (Exception e){
                System.err.println("Error: Line " + i + ", From third line the third argument (with ';' as separator) should be a java.math.BigDecimal " + e);
                System.exit(0);
            }

            if (exchange_rate.compareTo(new java.math.BigDecimal(0)) <= 0) {
                System.err.println("Error: Line " + i + ", exchange rate " + exchange_rate + " should not be negative or equal to 0");
                System.exit(0);
            }
            ccy_pairs.add(new Tuple<Integer,Integer,java.math.BigDecimal>(from_ccy, to_ccy, exchange_rate));
        }

        return ccy_pairs;
    }
    @SuppressWarnings("unchecked")
    private void set_ccy_ids()
    {
        // assign id 0, 1, 2 etc to each different ccy
        int id = 0;
        for (int ccy : different_currencies) {
            ccy_id.put(ccy, id++);
        }
        v = id;// above variable that specifies the number of different currencies

        vertex = (ArrayList<CurrencyNode>[])new ArrayList<?>[v];
        for (int i = 0; i < id; i++)
            vertex[i] = new ArrayList<CurrencyNode>();
    }

    private void get_currencies_to_convert(List<String> lines)
    {
        ArrayList<String> tickers = new ArrayList<String>(Arrays.asList(lines.get(0).split(";")));

        initial_ccy = ccy_id.get(make_guid(tickers.get(0)));
        desired_ccy = ccy_id.get(make_guid(tickers.get(2)));
        notional = new BigDecimal(tickers.get(1));
        notional.setScale(4, RoundingMode.HALF_UP);


        //    Console.WriteLine($"You want to convert {notional} {tickers[0]}-{initial_ccy} to {tickers[2]}-{desired_ccy}");

    }

    @SuppressWarnings("deprecation")
    private void build_graph(List<Tuple<Integer,Integer,java.math.BigDecimal>> ccy_pairs)
    {
        for (var ccy_pair : ccy_pairs) {
            int from_ccy_id = ccy_id.get(ccy_pair.x);
            int to_ccy_id = ccy_id.get(ccy_pair.y);
            java.math.BigDecimal xch_rate = ccy_pair.z;

            if (xch_rate.compareTo(new java.math.BigDecimal(0)) == 0) {
                System.err.println("Error: exchange rate cannot be equal to 0");
                System.exit(0);
            }
            // add vertex to the graph

            vertex[from_ccy_id].add(new CurrencyNode(to_ccy_id, xch_rate));
            vertex[to_ccy_id].add(new CurrencyNode(from_ccy_id, new java.math.BigDecimal(1).divide(xch_rate, 4, RoundingMode.HALF_UP)));
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
            System.err.println("Impossible to convert as missing exchange rate pair(s)");
            System.exit(0);
        }

        ArrayList<CurrencyNode> path = new ArrayList<CurrencyNode>(get_shortest_path(currencyNode));

        for (int i = path.size() - 1; i >= 0; i--) {
            BigDecimal currentExchangeRate = path.get(i).getExchangeRate();
            notional = notional.multiply(currentExchangeRate);
        }

        System.out.println(notional.setScale(0, RoundingMode.HALF_UP));
    }

    private boolean BFS(CurrencyNode[] currencyNode)
    {
        Queue<Integer> queue = new LinkedList<Integer>();
        for (int i = 0; i < v; i++) {
            currencyNode[i] = new CurrencyNode();
        }

        currencyNode[initial_ccy].setVisited(true);
        currencyNode[initial_ccy].setDistance(0);
        queue.add(initial_ccy);

        while (queue.size() != 0) {
            int u = queue.peek();
            queue.remove();
            for (int i = 0; i < vertex[u].size(); i++) {
                int current_ccy = vertex[u].get(i).getCurrencyID();

                if (currencyNode[current_ccy].getVisited() == false) {
                    currencyNode[current_ccy].SetVisited(currencyNode[u].getDistance(), u, vertex[u].get(i).getExchangeRate());
                    queue.add(current_ccy);

                    if (current_ccy == desired_ccy) {
                        return true; // We stop BFS when we reach the desired currency.
                    }
                }
            }
        }
        return false;
    }

    private static ArrayList<CurrencyNode> get_shortest_path(CurrencyNode[] currencyNode)
    {
        ArrayList<CurrencyNode> path = new ArrayList<CurrencyNode>();
        int i = desired_ccy;

        path.add(new CurrencyNode(i, new java.math.BigDecimal(1)));

        while (currencyNode[i].CurrencyID != -1) {
            var node = new CurrencyNode();
            node.setCurrencyID(currencyNode[i].getCurrencyID());
            node.setExchangeRate(currencyNode[i].getExchangeRate());
            path.add(node);
            i = currencyNode[i].getCurrencyID();
        }
        return path;
    }
}
