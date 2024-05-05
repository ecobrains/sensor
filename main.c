// Author: @benjamin 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cJSON.h"
#include <curl/curl.h>

// My Libraries
#include <sensorIdentificator> // Code to identify which sensor call APIs
#include <structWaste> // Waste object contain char type[20] and int quantity
#include <sensorData>  // Function to read data from sensor
#include <URLecobrainsPortal> // Url to website portal with data
#include <timePlanning> // Import a ENV Var to set time of report, based on cash plannig



// Function to send data to the web server via API
void send_report_to_webserver(Waste waste) {
    CURL *curl;
    CURLcode res;
    char *data;
    cJSON *json_data = cJSON_CreateObject();
    cJSON_AddStringToObject(json_data, "waste_type", waste.type);
    cJSON_AddNumberToObject(json_data, "quantity", waste.quantity);
    data = cJSON_PrintUnformatted(json_data);
    cJSON_Delete(json_data);

    // Initializing libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URLecobrainsPortal + "/api/report/submit_report");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, sensorIdentificator); // Identify which sensor
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "Error sending report: %s\n", curl_easy_strerror(res));
        else
            printf("Report successfully sent to the web server.\n");
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    free(data);
}

int main() {
    // Send the report every x minutes, where x is the ENV Var
    while (1) {
        Waste waste;
        sensorData()(&waste);
        send_report_to_webserver(waste);
        sleep(timePlanning); // Wait for x minutes before sending the next report,where x is the ENV Var
    }

    return 0;
}
