#include "order_book.h"
#include <iostream>

int main()
{
    OrderBook ob;
    ob.add_order({"strategy1", 100, 50.5});
    ob.save_to_file("orders.txt");
    std::cout << "Order added and saved to file.\n";
    return 0;
}