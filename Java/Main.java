import java.io.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.math.*;


/*
** COMPILE: javac -Xlint Main.java CurrencyNode.java Graph.java Tuple.java
** RUN: time java Main ../test_files/example.txt
*/


class Main {
    public static int check_number_of_ccy_pairs(List<String> lines)
    {
        if (lines.size() < 3)
        {
            System.err.println("Too small file");
            display_usage();
            System.exit(0);
        }


        try {
            int expected_ccy_pairs = Integer.parseInt(lines.get(1));

            if (expected_ccy_pairs < 0)
            {
                System.err.println("Second line of the file " + expected_ccy_pairs + " should be a positive number indicating the number of ccy pairs.");
                System.exit(0);
            }
            if (expected_ccy_pairs != lines.size() - 2)
            {
                System.err.println("Number of expected pairs does not match provided ccy pairs:"
                + "Expected ccy pairs: " + expected_ccy_pairs
                + "Actual ccy pairs: " + (lines.size() - 2));
                System.exit(0);
            }

            return expected_ccy_pairs;
        }
        catch (Exception e){
            System.err.println("Second line of the file should be an integer " + e);
            System.exit(0);
        }
        return 0;
    }

    public static List<String> parse_file(String filename)
    {
        List<String> lines = Collections.emptyList();
        try {
            lines = Files.readAllLines(Paths.get(filename), StandardCharsets.UTF_8);
        }

        catch (IOException e) {
            e.printStackTrace();
        }
        /*Iterator<String> itr = lines.iterator();
          while (itr.hasNext())
            System.out.println(itr.next());*/
        return lines;
    }

    static void display_usage()
    {
        System.out.println("Please provide a file as input"
        + "File should be formatted the following way:"
        + "EUR;550;JPY\n6\nAUD;CHF;0.9661\nJPY;KRW;13.1151\nEUR;CHF;1.2053"
        + "AUD;JPY;86.0305\nEUR;USD;1.2989\nJPY;INR;0.6571"
        + "First line Means that you want to convert 550 EUR to JPY"
        + "Second line means that you will provide 6 unique exchange rates"
        + "Following lines are the exchange rate, for example 4th line means that you can exchange 1 JPY for 13.1151 KRW\n");
    }

  public static void main(String args[])
  {
      if (args == null || args.length == 0) {
          display_usage();
      }

      for (int i = 0; i < args.length; i++) {
          String filename = args[i];
         // System.out.println(filename);
          ArrayList<String> lines = new ArrayList<String>(parse_file(filename)); // each lines of the file
          check_number_of_ccy_pairs(lines); // number of expected lines of currency pairs
          Graph graph = new Graph();
          graph.create_graph(lines);
          graph.solve_graph(); // shortest path
        }
    }
}
