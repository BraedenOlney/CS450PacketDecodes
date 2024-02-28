#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct EthHeader {
    uint8_t destinationMACAddress[6];
    uint8_t sourceMACAddress[6];
    uint16_t ethType;
};

int main(int argc, char* argv[]) {

    // Value to Return
    int valueToReturn = 0;

    // Check if a File was entered if not change return value and print error
    if (argc < 2) {
        valueToReturn = 1;
        fprintf(stderr, "Please provide a file name as the first argument");
    }

    // Open File
    FILE* inFile = fopen(argv[1], "rb");

    // check if file opened if not Change return value and print error
    if (inFile == NULL) {
        valueToReturn = 1;
        fprintf(stderr, "Error opening file");
    }

    // initalize the structure
    struct EthHeader ethHeader;

    // file has date extract it
    if (fread(&ethHeader, sizeof(ethHeader), 1, inFile) == 1) {

        // Shift bits to get type
        uint16_t etherType = (ethHeader.ethType >> 8) | (ethHeader.ethType << 8);

        // Print console header
        printf("Ethernet header:\n");
        printf("----------------\n");

        // Print the Destination MAC
        printf("Destination MAC address:\t%02x:%02x:%02x:%02x:%02x:%02x\n",
            ethHeader.destinationMACAddress[0], ethHeader.destinationMACAddress[1], ethHeader.destinationMACAddress[2],
            ethHeader.destinationMACAddress[3], ethHeader.destinationMACAddress[4], ethHeader.destinationMACAddress[5]
            );

        // Print Source MAC
        printf("Source MAC address:\t\t%02x:%02x:%02x:%02x:%02x:%02x\n",
            ethHeader.sourceMACAddress[0], ethHeader.sourceMACAddress[1], ethHeader.sourceMACAddress[2],
            ethHeader.sourceMACAddress[3], ethHeader.sourceMACAddress[4], ethHeader.sourceMACAddress[5]
            );

        // Print ethernet Type
        printf("Type:\t\t\t\t%04x\n", etherType);

        // Print Header for payload
        printf("\nPayload:\n");

        // initialize a current byte to print var
        uint8_t currentByte;

        // initialize a current position var to track position in the file 
        int currentPosition = 0;

        // loop while there is a next byte to print
        while (fread(&currentByte, sizeof(currentByte), 1, inFile) == 1) {

            // print two number from the current byte
            printf("%02x", currentByte);

            // increment position
            currentPosition++;

            // switch for spaces
            // if posistion is 8,16,24 they print two spaces for a break in the text
            // if 32 then print a new line to conitue the print format
            // else print a space inbetween the bytes
            switch (currentPosition) {
            case 8:
                printf("  ");
                break;
            case 16:
                printf("  ");
                break;
            case 24:
                printf("  ");
                break;
            case 32:
                printf("\n");
                currentPosition = 0;
                break;
            default:
                printf(" ");
            }
        }
    }
    else { // else print error
        printf("No data found");
        valueToReturn = 1;
    }

    // close file
    fclose(inFile);

    // return status
    return valueToReturn;
}
