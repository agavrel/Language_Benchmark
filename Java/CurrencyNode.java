public class CurrencyNode
{
    private boolean Visited;
    public boolean getVisited() { return this.Visited; }
    public void setVisited(boolean Visited) { this.Visited = Visited; }

    private int Distance; // distance from initial_ccy
    public int getDistance() { return this.Distance; }
    public void setDistance(int Distance) { this.Distance = Distance; }

    public int CurrencyID;
    public int getCurrencyID() { return this.CurrencyID; }
    public void setCurrencyID(int CurrencyID) { this.CurrencyID = CurrencyID; }

    private java.math.BigDecimal Exchange_rate;
    public java.math.BigDecimal getExchangeRate() { return this.Exchange_rate; }
    public void setExchangeRate(java.math.BigDecimal Exchange_rate) { this.Exchange_rate = Exchange_rate; }

    public CurrencyNode(int currencyID, java.math.BigDecimal exchange_rate) {
      CurrencyID = currencyID;
      Exchange_rate = exchange_rate;
    }

    public CurrencyNode() {
      Visited = false;
      Distance = (int) Character.MAX_VALUE;
      CurrencyID = -1;
      Exchange_rate = new java.math.BigDecimal(1);
    }

    public void SetVisited(int distance, int currencyID, java.math.BigDecimal exchange_rate) {
        Visited = true;
        Distance = distance + 1;
        CurrencyID = currencyID;
        Exchange_rate = exchange_rate;
    }
}
