#include "CoinMarketCapApi.h"

CoinMarketCapApi::CoinMarketCapApi(HTTPClient &client, String apiKey)	{
  this->client = &client;
  this->COINMARKET_API_KEY = apiKey;
}

String CoinMarketCapApi::SendGetToCoinMarketCap(String command) {
  String body;
  client->begin(COINMARKETCAP_HOST+command);
  int httpCode = client->GET();                                        //Make the request
  if (httpCode > 0) { //Check for the returning code
      body = client->getString();
    }

  else {
    Serial.println("Error on HTTP request");
  }

  client->end(); //Free the resources
  return body;
}

CMCTickerResponse CoinMarketCapApi::GetTickerInfo(String coinId, String currency) {
 String command="/v1/cryptocurrency/quotes/latest?symbol=" + coinId + "&CMC_PRO_API_KEY="+COINMARKET_API_KEY;
  if (currency != "") {
    command = command + "&convert=" + currency;
  }
  String response = SendGetToCoinMarketCap(command);
  Serial.println(response);
  CMCTickerResponse responseObject = CMCTickerResponse();
  DynamicJsonDocument root(1024);
  DeserializationError error = deserializeJson(root, response);

  if(error){
    responseObject.error = error.c_str();
  }else{
    responseObject.id = root["data"][coinId]["id"].as<String>();
    responseObject.name = root["data"][coinId]["name"].as<String>();
    responseObject.symbol = root["data"][coinId]["symbol"].as<String>();
    responseObject.rank =  root["data"][coinId]["cmc_rank"].as<int>();
    responseObject.total_supply =root["data"][coinId]["total_supply"].as<double>();
    responseObject.price_eur = root["data"][coinId]["quote"][currency]["price"].as<double>();
    responseObject.percent_change_1h =  root["data"][coinId]["quote"][currency]["percent_change_1h"].as<double>();
    responseObject.percent_change_24h =  root["data"][coinId]["quote"][currency]["percent_change_24h"].as<double>();
    responseObject.percent_change_7d =  root["data"][coinId]["quote"][currency]["percent_change_7d"].as<double>();
    responseObject.last_updated =  root["data"][coinId]["quote"][currency]["last_updated"].as<double>();
    responseObject.market_cap_eur = root["data"][coinId]["quote"][currency]["market_cap"].as<double>();
  }

  return responseObject;
}

void CoinMarketCapApi::closeClient() {
  if(client->connected()){
    client->end();
  }
}
