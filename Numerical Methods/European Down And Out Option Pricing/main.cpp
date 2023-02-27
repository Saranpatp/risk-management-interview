#include <iostream>
#include <chrono>
#include <random>
#include <cmath>

using namespace std;
default_random_engine generator;

double duration, rf, volatility, initial_stock_price ,K, B; // B Barrier Price
int n, m; // number of trails , number of sample point;

double max(double a, double b)
{
    return (b < a) ? a : b;
}

double getUniform() {
    uniform_real_distribution <double> distribution(0, 1.0);
    return distribution(generator);

}
// Reference from Prof R.S. material
double N(const double& z) {
    if (z > 6.0) { return 1.0; }; // this guards against overflow
    if (z < -6.0) { return 0.0; };
    double b1 = 0.31938153;
    double b2 = -0.356563782;
    double b3 = 1.781477937;
    double b4 = -1.821255978;
    double b5 = 1.330274429;
    double p = 0.2316419;
    double c2 = 0.3989423;
    double a = fabs(z);
    double t = 1.0 / (1.0 + a * p);
    double b = c2 * exp((-z) * (z / 2.0));
    double n = ((((b5 * t + b4) * t + b3) * t + b2) * t + b1) * t;
    n = 1.0 - b * n;
    if (z < 0.0) n = 1.0 - n;
    return n;
};

double optionPricePutBlackScholes(const double& S,      // spot price
                                      const double& K,      // Strike (exercise) price,
                                      const double& r,      // interest rate
                                      const double& sigma,  // volatility
                                      const double& time)
{
    double time_sqrt = sqrt(time);
    double d1 = (log(S / K) + r * time) / (sigma * time_sqrt) + 0.5 * sigma * time_sqrt;
    double d2 = d1 - (sigma * time_sqrt);
    return K * exp(-r * time) * N(-d2) - S * N(-d1);
};

double optionPriceCallBlackScholes(const double& S,       // spot (underlying) price
                                       const double& K,       // strike (exercise) price,
                                       const double& r,       // interest rate
                                       const double& sigma,   // volatility
                                       const double& time)	  // time to maturity
{
    double time_sqrt = sqrt(time);
    double d1 = (log(S / K) + r * time) / (sigma * time_sqrt) + 0.5 * sigma * time_sqrt;
    double d2 = d1 - (sigma * time_sqrt);
    return S * N(d1) - K * exp(-r * time) * N(d2);
};


double closedFormDownAndOutEuropeanCallOption()
{
    // I took this formula from Wilmott, Howison and Dewynne, "The Mathematics of Financial Derivatives"
    float X = (2 * rf) / (volatility * volatility);
    float A = optionPriceCallBlackScholes(initial_stock_price, K,
                                              rf, volatility, duration);
    float B = (B * B) / initial_stock_price;
    float C = pow(initial_stock_price / B, -(X - 1));
    float D = optionPriceCallBlackScholes(B, K, rf, volatility, duration);
    return (A - D * C);
}

double closedFormDownAndInEuropeanPutOption()
{
    // just making it easier by renaming the global variables locally
    double S = initial_stock_price;
    double r = rf;
    double T = duration;
    double sigma = volatility;
    double H = B;
    double X = K;

    // Took these formula from some online reference
    double lambda = (r + ((sigma * sigma) / 2)) / (sigma * sigma);
    double temp = 2 * lambda - 2.0;
    double x1 = (log(S / H) / (sigma * sqrt(T))) + (lambda * sigma * sqrt(T));
    double y = (log(H * H / (S * X)) / (sigma * sqrt(T))) + (lambda * sigma * sqrt(T));
    double y1 = (log(H / S) / (sigma * sqrt(T))) + (lambda * sigma * sqrt(T));
    return (-S * N(-x1) + X * exp(-r * T) * N(-x1 + sigma * sqrt(T)) +
            S * pow(H / S, 2 * lambda) * (N(y) - N(y1)) -
            X * exp(-r * T) * pow(H / S, temp) * (N(y - sigma * sqrt(T)) - N(y1 - sigma * sqrt(T))));
}

double closedFormDownAndOutEuropeanPutOption()
{
    double vanilla_put = optionPricePutBlackScholes(initial_stock_price, K,
                                                        rf, volatility, duration);
    double put_down_in = closedFormDownAndInEuropeanPutOption();
    return (vanilla_put - put_down_in);
}

void updateOptionPrices(vector<double>& stock_prices,vector<bool>& coefficient, double &curr_option_price, double &strike_price,bool is_call){
    for (int i = 0; i < stock_prices.size(); i++) {
        curr_option_price+= coefficient[i] * max(0.0 ,(strike_price - stock_prices[i])*pow(-1,(double) is_call));
    }
}

double getPc(double& stock_price){
    double pc = exp((-2.0 / pow(volatility,2) * duration)) * log(initial_stock_price / B)*log(stock_price / B);
    if (initial_stock_price > B and stock_price > B) return  (1.0 - pc);
    return 0;
}

void updateAdjOptionPrices(vector<double>& stock_prices, double& curr_adj_option_price, double &strike_price,bool is_call){
    for (int i = 0; i < stock_prices.size(); i++){
        curr_adj_option_price += max(0.0,(strike_price - stock_prices[i]) * pow(-1,(double) is_call))* getPc(stock_prices[i]);
    }

}


int main(int argc, char* argv[]) {
    sscanf(argv[1], "%lf", &duration);
    sscanf(argv[2], "%lf", &rf);
    sscanf(argv[3], "%lf", &volatility);
    sscanf(argv[4], "%lf", &initial_stock_price);
    sscanf(argv[5], "%lf", &K);
    sscanf(argv[6], "%d", &n);
    sscanf(argv[7], "%d", &m);
    sscanf(argv[8], "%lf", &B);

//    duration = 1;
//    rf = 0.05;
//    volatility = 0.25;
//    initial_stock_price = 50;
//    K = 40;
//    n = 1000000;
//    m = 1000;
//    B = 20;

    cout << "European Down and Out Continuous Barrier Options Pricing via Monte Carlo Simulation" << endl;
    cout << "Expiration Time (Years) = " << duration << endl;
    cout << "Risk Free Interest Rate = " << rf << endl;
    cout << "Volatility (%age of stock value) = " << volatility * 100 << endl;
    cout << "Initial Stock Price = " << initial_stock_price << endl;
    cout << "Strike Price = " << K << endl;
    cout << "Barrier Price = " << B << endl;
    cout << "Number of Trials = " << n << endl;
    cout << "Number of Divisions = " << m << endl;
    cout << "--------------------------------------" << endl;

    double call_price, put_price, call_adj_price, put_adj_price;
    call_price= put_price= call_adj_price= put_adj_price = 0;

    double dt = duration/((double) m);
    double dr = (rf-0.5*pow(volatility,2)) * dt;
    double dvoltility = volatility*sqrt(dt);

    for (int i = 0; i < n; i++){
        vector<double> stock_prices(4,initial_stock_price);
        vector<double> adj_prices(4,initial_stock_price);

        vector<bool> coefficient(4,1); // will be 0 when stock_price <= B
        for(int j = 0; j<m;j++){
            // getting normal dist using box-muller
            double x = getUniform();
            double y = getUniform();
            double a = sqrt(-2.0 * log(x)) * cos(2*M_PI * y);
            double b = sqrt(-2.0 * log(x)) * sin(2*M_PI  * y);
            vector<double> box_muller = {a,b};

            for (int l = 0; l < stock_prices.size(); l++){
                int tmp = l/2;
                stock_prices[l] = stock_prices[l]*exp(dr + dvoltility*box_muller[tmp]*pow(-1,l));
                if (stock_prices[l] <= B) coefficient[l]=false; //update coef vector
            }
        }
        double curr_call_price, curr_put_price, curr_call_adj_price, curr_put_adj_price;
        curr_call_price= curr_put_price= curr_call_adj_price= curr_put_adj_price = 0.0;

        updateOptionPrices(stock_prices,coefficient,curr_call_price,K, true);
        updateOptionPrices(stock_prices,coefficient,curr_put_price,K, false);

        updateAdjOptionPrices(stock_prices,curr_call_adj_price,K,true);
        updateAdjOptionPrices(stock_prices,curr_put_adj_price,K,false);

        //discounted avg prices
        call_price += exp(-rf * duration) * (curr_call_price / 4.0);
        put_price += exp(-rf * duration) * (curr_put_price / 4.0);
        call_adj_price += exp(-rf * duration) * (curr_call_adj_price / 4.0);
        put_adj_price += exp(-rf * duration) * (curr_put_adj_price / 4.0);

    }

    cout << "--------------------------------------" << endl;
    cout << "The average Call Price by explicit simulation  = " << call_price/(double) n  << endl;
    cout << "The call price using the (1-p)-adjustment term = " << call_adj_price/(double) n << endl;
    cout << "Theoretical Call Price                         = " << closedFormDownAndOutEuropeanCallOption() << endl;
    cout << "--------------------------------------" << endl;
    cout << "The average Put Price by explicit simulation   = " << put_price/(double) n << endl;
    cout << "The put price using the (1-p)-adjustment term  = " << put_adj_price/(double) n << endl;
    cout << "Theoretical Put Price                          = " << closedFormDownAndOutEuropeanPutOption() << endl;
    cout << "--------------------------------------" << endl;
    return 0;
}
