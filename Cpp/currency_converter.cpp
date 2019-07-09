#include <iostream> // for std::cerr  and std::cout
#include <fstream> // to read input file
#include <vector>
#include <regex>
#include <list>
#include <unordered_map> // to check for duplicate exchange rate
#include <unordered_set> // to count number of different currencies
#include <cstdint> // to convert char array to uint_32
#include <cmath> // for HUGE_VAL
#include <limits> // for INT32_MAX
#include <iomanip> // for setprecision

/*
** TODO for later optimization: read about "Faster all-pairs shortest paths via circuit complexity∗" https://arxiv.org/pdf/1312.6680.pdf
** COMPILE: g++ -D DEBUG=false -std=c++1z currency_converter.cpp -o cpp_binary
** RUN: time ./cpp_binary ../test_files/example.txt
** COMPILE FOR WINDOWS: sudo apt-get install mingw-w64 && /usr/bin/x86_64-w64-mingw32-g++ currency_converter.cpp -o cpp_binary.exe
*/

using namespace std;

#ifndef DEBUG
#define DEBUG false // ifndef to allow to change value through -D option while compiling with gcc
#endif

/*
** helper functions
*/

bool isPositiveNumber(const std::string &str) {
  return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
}

bool isPositiveDouble(const std::string &str) {
  return !str.empty() && str.find_first_not_of(".0123456789") == string::npos;
}

unsigned long get_ccy_pairs_nb(const std::string &expected_ccy_pairs, unsigned long len)
{
    if (!isPositiveNumber(expected_ccy_pairs))
    {
        std::cerr << "Second line of the file '" << expected_ccy_pairs << "' should be a positive number indicating the number of ccy pairs." <<std::endl;
        return 0;
    }

    unsigned long ccy_pairs = std::stoul(expected_ccy_pairs,nullptr,0);

    if (ccy_pairs != len)
    {
        std::cerr << "Number of expected pairs does not match provided ccy pairs: \n" \
        << "Expected ccy pairs: " << ccy_pairs << "\nActual ccy pairs: " << len << std::endl;
		return 0;
    }

    return ccy_pairs;
}


bool read_file(std::vector<string> & vecOfStrs, char *av)
{
    std::ifstream in(av);
    if(!in)
	{
		std::cerr << "Cannot open the File : "<< av[1] <<std::endl;
		return false;
	}

    std::string str;
    	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str))
	{
		// Line contains string of length > 0 then save it in vector
		if(str.size() > 0)
			vecOfStrs.push_back(str);
	}
	//Close The File
	in.close();
    return true;
}

bool all__not_alpha( const std::string& str )
{
    // // http://www.stroustrup.com/C++11FAQ.html#for
    // http://en.cppreference.com/w/cpp/string/byte/isalpha
    for( char c : str ) if( !std::isalpha(c) ) return true ;

    return false ;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
    }

    return elems;
}

/*
** Solving algo
*/

bool BFS(vector<pair<int32_t,long double>> adj[], int src, int dest, int v, pair <int32_t, long double> pred[], int dist[])
{
    // a queue to maintain queue of vertices whose adjacency list is to be
    // scanned as per normal DFS algorithm
    list<int> queue;

    // boolean array visited[] which stores the information whether ith vertex is reached
    // at least once in the Breadth first search
    bool visited[v];

    // initially all vertices are unvisited so v[i] for all i is false
    // and as no path is yet constructed dist[i] for all i set to maximum value
    for (int i = 0; i < v; i++) {
        visited[i] = false;
        dist[i] = INT32_MAX;
        pred[i].first = -1;
    }

    // now source is first to be visited and distance from initial currency to itself should be 0
    visited[src] = true;
    dist[src] = 0;
    queue.push_back(src);

    // standard BFS algorithm
    while (!queue.empty()) {
        int u = queue.front();
        queue.pop_front();
        for (int i = 0; i < adj[u].size(); i++) {
            if (visited[adj[u][i].first] == false) {
                visited[adj[u][i].first] = true;
                dist[adj[u][i].first] = dist[u] + 1;
                pred[adj[u][i].first].first = u; // save the to_currency
                pred[adj[u][i].first].second = adj[u][i].second; // save the exchange rate
                queue.push_back(adj[u][i].first);

                // We stop BFS when we find destination.
                if (adj[u][i].first == dest)
                   return true;
            }
        }
    }

    return false;
}

void solve_graph(vector<pair<int32_t,long double>> adj[], int32_t src, int32_t dest, int v, long double notional)
{
    // predecessor[i] array stores predecessor of i and distance array stores distance of i from src
    pair <int32_t, long double>  pred[v];
    int dist[v];

    if (BFS(adj, src, dest, v, pred, dist) == false)
    {
        std::cerr << "Impossible to solve as missing exchange rate pair(s)" << std::endl;
        return ;
    }

    // vector path stores the shortest path
    vector<pair<int32_t, long double>> path;
    int crawl = dest;

    path.push_back(std::make_pair(crawl, 1));
    while (pred[crawl].first != -1) {
        path.push_back(std::make_pair(pred[crawl].first, pred[crawl].second));
        crawl = pred[crawl].first;
    }

    #if DEBUG
        // distance from source is in distance array
        cout << "Shortest path length is : " << dist[dest];
        // printing path from source to destination
        cout << "\nPath is:\n";
        for (int i = path.size() - 1; i >= 0; i--)
            cout << path[i].first << " (" << path[i].second << ")    ";
        cout << "\n";
    #endif

    for (int i = path.size() - 1; i >= 0; i--)
    {
        stringstream tmp;
        tmp << setprecision(4) << fixed << path[i].second;
        notional = notional * stod(tmp.str());   // new_val = 3.143
        tmp.str(string());                  // clear tmp for future use

        #if DEBUG
            cout << notional << " " << path[i].second << "\n";
        #endif
    }
    /* quite stupid, but requested, however it would be way faster to do the following:
    for (int i = path.size() - 1; i >= 0; i--)
        notional *= path[i].second
    */

    cout << round(notional) << "\n";
}

/*
* function to transform the 3 letters of the ticker to an int
*/

int32_t make_guid(char ticker[3]) {
    return ((ticker[0] << 16) | (ticker[1] << 8) | ticker[2]);
}

/*
** main > get_currencies > create_graph > solve_graph
*/

/*
** helper for create_graph function : create links between each currency
*/

void add_vertex(vector<pair<int32_t,long double>> adj[], int32_t src, int32_t dest, long double xch_rate)
{
    adj[src].push_back(make_pair(dest, xch_rate));
    adj[dest].push_back(make_pair(src, 1 / xch_rate)); // we also add the other way around with the inverse of the exchange rate

    #if DEBUG
        cout << adj[src][0].second  << "-" << adj[dest][0].second << "\n";
    #endif
}

void create_graph(std::unordered_set<int32_t> currencies, std::vector<string> lines, \
    std::vector<std::string> & tokens) {
    char current_ccy[3];
    char desired_ccy[3];

    // assign id 0, 1, 2 etc to the guid of each ccy
    std::unordered_map<std::int32_t,int32_t> ccy_id;
    int32_t i = 0;
    for (const auto& elem: currencies) {
        ccy_id[elem] = i++;
    }

    vector<pair<int32_t,long double>> exchange_rates[currencies.size()];

    for (int i = 2; i < lines.size(); i++)
    {
        tokens = split(lines[i], ';');
        std::strncpy(current_ccy, tokens[0].c_str(), 3);
        std::strncpy(desired_ccy, tokens[1].c_str(), 3);

        long double xch_rate = stold(tokens[2].c_str(), 0);

        add_vertex(exchange_rates, ccy_id[make_guid(current_ccy)], ccy_id[make_guid(desired_ccy)], xch_rate);

        #if DEBUG
            cout << current_ccy  << "-" << desired_ccy << "   " <<
            ccy_id[make_guid(current_ccy)] << "-" << ccy_id[make_guid(desired_ccy)] << "\n";
        #endif
    }

    tokens = split(lines[0], ';');
    uint32_t amount = stoul(tokens[1],  0);
    std::strncpy(current_ccy, tokens[0].c_str(), 3);
    std::strncpy(desired_ccy, tokens[2].c_str(), 3);

    // special case (i.e : USD to USD)
    if (strcmp(current_ccy, desired_ccy) == 0)
    {
        cout << amount << "\n";
        return ;
    }
    solve_graph(exchange_rates, ccy_id[make_guid(current_ccy)], ccy_id[make_guid(desired_ccy)], currencies.size(), amount);
}

/*
** functions to parse inputs of exchange rate pairs AND store tickers in a std::unordered_set
*/

bool get_currencies(std::unordered_set<int32_t> & currencies, \
std::vector<string> vecOfStrs, std::vector<std::string> & tokens) {

    char current_ccy[3];
    char desired_ccy[3];
    std::map<int32_t, int8_t> exchange_rates;

    for (int i = 2; i < vecOfStrs.size(); i++)
    {
        tokens = split(vecOfStrs[i], ';');

        // check for valid tickers (length == 3 and alpha characters only)
        if (tokens[0].length() != 3 || tokens[1].length() != 3)
        {
            std::cerr << "Length of all currency tickers should be equal to 3\n";
            return false;
        }
        if (all__not_alpha(tokens[0]) || all__not_alpha(tokens[1]))
        {
            std::cerr << "All tickers should be only alpha characters\n";
            return false;
        }
        std::strncpy(current_ccy, tokens[0].c_str(), 3);
        std::strncpy(desired_ccy, tokens[1].c_str(), 3);

        // store currencies as integer unique value amd store them in a set
        currencies.insert(make_guid(current_ccy));
        currencies.insert(make_guid(desired_ccy));
        // make sure that we don't have duplicate exchange rates
        if (exchange_rates[make_guid(current_ccy) * make_guid(desired_ccy)] & 1)
        {
            std::cerr << "Err: Duplicate exchange rate: "<< current_ccy
            << "\nfor example either EUR CHF twice or also CHF EUR and EUR CHF are not accepted as input, please edit/correct the input file\n";
            return false;
        }
        exchange_rates[make_guid(current_ccy) * make_guid(desired_ccy)] = 1;

        // make sure that we have a positive float number
        if (!isPositiveDouble(tokens[2]))
        {
            std::cerr << "Exchange rate should be a positive (float) number\nErr Line " << i << ": "<< tokens[2] << "\n";
            return false;
        }
        long double exchange_rate = stold(tokens[2].c_str(), 0);
        if (!exchange_rate)
        {
            std::cerr << "Exchange rate equal to 0, really?\nErr Line " << i << ": "<< tokens[2] << "\n";
            return false;
        }
        #if DEBUG
            cout << current_ccy << desired_ccy << exchange_rate << "\n";
        #endif
    }

    #if DEBUG
        cout << "Number of different currencies: " << currencies.size() << "\n";
    #endif

    return true;
}

void display_usage()
{
    std::cerr << "Provide a file as input\n"
    << "File should be formatted the following way:\n"
    << "EUR;550;JPY\n6\nAUD;CHF;0.9661\nJPY;KRW;13.1151\nEUR;CHF;1.2053 \
    \nAUD;JPY;86.0305\nEUR;USD;1.2989\nJPY;INR;0.6571\n"
    << "First line Means that you want to convert 550 EUR to JPY\n"
    << "Second line means that you will provide 6 unique exchange rates\n"
    << "Following lines are the exchange rate, for example 4th line means that you can exchange 1 JPY for 13.1151 KRW\n";
}

/*
** main program
*/

int main(int ac, char **av)
{
    if (ac < 2)
    {
        display_usage();
        return 1;
    }

    std::vector<string> lines; // to parse each line of the input file
    std::vector<string> tokens; // used to get the 3 items of each line
    while (--ac)
    {
        lines.clear();
        tokens.clear();
        if (!read_file(lines, av[ac])) // store input file in a vector of string "lines"
            return 1;
        if (!get_ccy_pairs_nb(lines[1], lines.size() - 2)) // if mistmatch between what is indicated on second line and exchange rates contained in the file
            return 1;

        unordered_set<int32_t> currencies;
        if (!get_currencies(currencies, lines, tokens))
            return 1;

        create_graph(currencies, lines, tokens);
    }

    return 0;
}
