import os
from math import floor
import numpy as np
import pandas as pd
import random
import string

# CONFIG
tickers = ["91282CFX4", "91282CFW6", "91282CFZ9", "91282CFY2", "91282CFV8", "912810TM0", "912810TL2"]
books = ["TRSY1", "TRSY2", "TRSY3"]
pricing_path = os.path.join(os.getcwd(), "bondpricingprovider", "prices.txt")
tradebooking_path = os.path.join(os.getcwd(), "bondtradebookingprovider", "trades.txt")
marketdata_path = os.path.join(os.getcwd(), "bondmarketdataprovider", "marketdata.txt")
inquiry_path = os.path.join(os.getcwd(), "bondinquiryprovider", "inquiries.txt")



def price2str(price):
    int_price = int(floor(price))
    int_str = str(int_price)
    xy_price = int(floor(32 * (price - int_price)))
    xy_str = str(xy_price) if xy_price > 9 else "0" + str(xy_price)
    z_price = int(floor(256 * (price - int_price - xy_price / 32)))
    z_str = str(z_price) if z_price != 4 else "+"
    return int_str + "-" + xy_str + z_str 

def gen_order_id():
    return ''.join(random.choices(string.ascii_uppercase +string.digits, k=10))

def gen_prices():
    prices = np.arange(99, 101+1/256, 1/256)

    prices_osc = np.concatenate((prices, np.flip(prices[1:-1])))
    price_size = len(prices_osc)

    num_prices = 1_000
    rows = [["product_id", "mid_price", "spread"]]
    for i, t in enumerate(tickers):
        for n in range(num_prices):
            rows.append([t,price2str(prices_osc[(num_prices * i + n)%price_size]), price2str(1/64 if (num_prices * i + n)%2 else 1/128)])

    pd.DataFrame(rows).to_csv(pricing_path, header=False, index=False)

    
def gen_trades():

    trades_sizes = [1_000_000 * i for i in range(1, 6)]

    rows = [["trade_id", "product_id", "book_id", "direction", "price", "quantity"]]
    num_trades = 10
    for i, t in enumerate(tickers):
        for n in range(num_trades):
            rows.append([gen_order_id(), t, books[(num_trades * i + n)%3], "SELL" if (num_trades * i + n)%2 else "BUY", "100-000" if (num_trades * i + n)%2 else "99-000", trades_sizes[(num_trades * i + n)%5]])
    pd.DataFrame(rows).to_csv(tradebooking_path, header=False, index=False)


def gen_market_data():
    prices = np.arange(99, 101+1/256, 1/256)
    spreads = np.arange(1/128, 5/128, 1/128)

    prices_osc = np.concatenate((prices, np.flip(prices[1:-1])))
    price_size = len(prices_osc)
    spreads_osc = np.concatenate((spreads, np.flip(spreads[1:-1])))
    spreads_size = len(spreads_osc)

    rows = [["product_id", "bid1", "ask1", "quantity1", "bid2", "ask2", "quantity2", "bid3", "ask3", "quantity3", "bid4", "ask4", "quantity4", "bid5", "ask5", "quantity5"]]
    num_order_books = 1_000
    for i, t in enumerate(tickers):
        for n in range(num_order_books):

            curr_price = prices_osc[(num_order_books * i + n) % price_size]
            curr_spreads = spreads_osc[(num_order_books * i + n) % spreads_size]
            rows.append([t, 
            price2str(curr_price - curr_spreads/2), price2str(curr_price + curr_spreads/2), "1000000",
            price2str(curr_price - curr_spreads/2  - 1 / 256), price2str(curr_price + curr_spreads/2 + 1 / 256), "2000000",
            price2str(curr_price - curr_spreads/2  - 2 / 256), price2str(curr_price + curr_spreads/2 + 2 / 256), "3000000",
            price2str(curr_price - curr_spreads/2  - 3 / 256), price2str(curr_price + curr_spreads/2 + 3 / 256), "4000000",
            price2str(curr_price - curr_spreads/2  - 4 / 256), price2str(curr_price + curr_spreads/2 + 4 / 256), "5000000",
            ])
    pd.DataFrame(rows).to_csv(marketdata_path, header=False, index=False)

def gen_inquiries():

    num_inquiries = 10
    rows = [["inquiry_id", "state", "product_id", "direction", "quantity"]]
    for i, t in enumerate(tickers):
        for n in range(num_inquiries):
            rows.append([gen_order_id(), "RECEIVED", t, "SELL" if (num_inquiries * i + n)%2 else "BUY", "1000000"])
    
    pd.DataFrame(rows).to_csv(inquiry_path, header=False, index=False)




def main():
    gen_prices()
    gen_trades()
    gen_market_data()
    gen_inquiries()

if __name__ == "__main__":
    main()