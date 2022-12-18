# MTH9815 Software for Financial Engineering: Final Project 
@ Nicolas Buchwalder

## Automatic Setup and Launch
The entire project can be compiled and run with command:
```
./setup.sh
```
The trading system takes about 5 minutes to read all the data

You can add arguments to that function for different configurations, do the following command to see the different configurations
```
./setup.sh -h
```
**WARNING:** this has only be tested on macos, if the setup does not work, please following the manual procedure.  
It is expected to automatically open terminals, do not be surprised by that!
## Manual Setup and Launch

1. (Optional) Delete the following executables:
    - dataproviders/bondinquiryprovider/bondinquiryprovider
    - dataproviders/bondmarketdataprovider/bondmarketdataprovider
    - dataproviders/bondtradebookingprovider/bondtradebookingprovider
    - dataproviders/bondpricingprovider/bondpricingprovider
    - tradingsystem/tradingsystem
    - tradinglogs/bondexecutionlog/bondexecutionlog
    - tradinglogs/bondstreaminglog/bondstreaminglog
2. Run the following command in the root folder
    ```
    make
    ```
3. Run all the executables from the list in the first step in different terminals. Order is important: first all the dataproviders must run, then trading system, then the two trading logs. Running the first one would be:
    ```
    ./dataproviders/bondinquiryprovider/bondinquiryprovider
    ```
## Requirements:
c++17 and boost 1.80
## Project Files and Folders
The project is structured as follows:
- setup.sh : bash file to run project
- dataproviders/ : folder containing the 4 independant reading processes
    - bondinquiryprovider/: project for the inquiry reading process
    - bondmarketdataprovider/: project for the marketdata reading process
    - bondtradebookingprovider/: project for the trades reading process
    - bondpricingprovider/: project for the prices reading process
- tradinglogs/ : folder containing the 2 independent writing processes
    - bondexecutionlog/: project for the execution writer
    - bondstreaminglog/: project for the streaming writer
- tradingsystem/ : folder for the main process of project 
    - genericclasses/ : contain the classes given in advance + connectors + algostream/execution
    - bondspecific/ : contains the classes implemented specifically for bond trading
    - data/ : folder where all the historical data and gui data of the trading system are written to and stored
- python/ : folder where the python scripts for data generation and cubic spline are stored (see "Bonds Data for more details")
    

## Documentation of project
### Modifications on given original tradingsystem code
#### SOA 
In soa.hpp, I modified the implementation of the Service class as follows:
1. I added a proctected unordered map member so that every derived service can identity its specific data (Product, OrderBook, PV01, etc..) automatically via the map
2. I added the following public methods:
    - ExistingData: checking if its already in the map
    - AddData: adding data to map
    - UpdateData: modifying data linked to key
    - GetData has been implemented here so it doesnt need to be set in derived classes
3. I only kept OnMessage method as pure here as some services do not have listeners (GUI for example). This prevents a bizarre design choice of setting the pure virtual functions to nothing in some derived classes
4. I created a ListenedService class which is derived from Service and that will be used by services that have listeners. It has a protected template vector of listeners, the AddListener and GetListeners are defined here so that they don't need to be defined in derived classes.
5. All given original generic classes now inherit from ListenedService instead of Service (except historical as it does not have listeners)
6. HistoricalDataService defined the OnMessage message as blank as its replaced by the persistdata method


#### Connectors
I created on connectors.hpp three abstract classes to define the three ways  connectors are used in this project: read from socket, publish to socket, publish to txt file. Derived classes only need to call the constuctor of those base classes and to define how they process (incoming or outcoming) data (so from string to datatype  for publish connectors or inversely for read connectors). We have then:
1. ReadSocketConnector:  the constructor has arguments display name (to print when waiting for connection etc), address and port number (to define socket) and then tries to connect with boost::asio::tcp
2. SocketPublishConnector: same constructor arguments, the only difference is it has an acceptor (waiting on connexion instead of trying to connect)
3. FilePublishConnector: constructor has argment display name for the same reasons as before and path name where data will be published

## Bond Specific Services
All have pretty much the same structure that is enforced from the Service, ServiceListener, and Connector classes. No listeners have the ProcessRemove function defined, services related to trades do not have ProcessUpdate as a trade is never modified.  
 Here are the notable design particularities:
1. BondTradeBookingService and BondHistoricalPostionService must be initialized with the vector of trading book names so that they can respectively decide how trades are attributed to books and store positions for all books to positions.txt
2. BondGUIService initializer has the throttle time (in seconds) and the number of lines as arguments to set the how quickly the file is cleared to new content of a certain number of lines
3. BondRiskService redefines the inherited vector of listener pointers to specifically a vector of HistoricalRiskServiceListener pointers (as well as its related methods AddListener and GetListeners) as this derived listener class has the inherited ProcessAdd and ProcessUpdate methods for PV01 of Bonds but also for PV01 of SectorBucket (which the base class does not have). It also has a OnMessageBucket to call its sectorbucket listeners.
## Parallel Processes
### Data Providers
The 4 data providers in the dataproviders profile only send data at a certain rate (defined by the refresh_rate_ms variable). Without that, the connectors that read the socket had errors (seems to be a bug of boost::asio?). I set the frequency of inquiries and trades at 1 per second (so refresh_rate_ms=1000) so that is happends all along the project. The others are set to 20 per second (so refresh_rate_ms=50). This can be changed if necessary  
    I only generated 1000 orderbooks and prices because of that rate so that the whole thing takes about 5 minutes
## Data Generation
I generated the data from the datagenerator.py script in the python folder
As said above, both the marketdata and prices files only have 7000 rows instead of 7m. 
## Bonds data
The CUSIP come from the following website: [https://treasurydirect.gov/auctions/auction-query/](https://treasurydirect.gov/auctions/auction-query/)
The coupon and PV01 data comes from the following website: [https://eiptrading.com/risk-management/](https://eiptrading.com/risk-management/)
The 20 year bond is missing from this website, I did a cubic spline interpolation to get estimate the value. This can be seen in the python folder PV01.ipynb file.

    

