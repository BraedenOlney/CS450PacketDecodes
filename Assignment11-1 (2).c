//Braeden Olney
//PacketDecode3 (Assignment 11)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct EthernetHeader {
    uint8_t destinationMACAddress[6];
    uint8_t sourceMACAddress[6];
    uint16_t ethType;
};

struct IPv4Header {
    uint8_t versionLength;
    uint8_t DSCPECN;
    uint16_t ipLength;
    uint16_t ipID;
    uint16_t offSetFlagFragment;
    uint8_t liveTime;
    uint8_t ipProtocol;
    uint16_t checkSum;
    uint32_t sourceAddress;
    uint32_t destinationAddress;
};

struct TCPHeader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    uint16_t dataOffsetFlag;
    uint16_t windowSize;
    uint16_t checkSum;
    uint16_t urgPointer;
};

int main(int argc, char* argv[]) {

    // Value to Return
    int valueToReturn = 0;

    // Check if a File was entered if not change return value and print error
    if (argc < 2) {
        fprintf(stderr, "Please provide a file name as the first argument");
        valueToReturn = 1;
    }

    // Open File
    FILE* inFile = fopen(argv[1], "rb");

    // check if file opened if not Change return value and print error
    if (inFile == NULL) {
        valueToReturn = 1;
        fprintf(stderr, "Error opening file");
    }

    // initalize the ethernet structure
    struct EthernetHeader ethHeader;


    // file has date extract it
    if (fread(&ethHeader, sizeof(ethHeader), 1, inFile) == 1) {

        // Shift bits to get type
        uint16_t etherType = (ethHeader.ethType >> 8) | (ethHeader.ethType << 8);

        // Print ethernet header
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

        // Print ethernet Type and converting the byte reading
        printf("Type:\t\t\t\t%04x\n\n", (ethHeader.ethType >> 8) | (ethHeader.ethType << 8));

        // Initialize the IP Header
        struct IPv4Header ipHeader;

        // if the first part of the data has been read read the ip header
        if (fread(&ipHeader, sizeof(ipHeader), 1, inFile) == 1) {

            // Print IP header
            printf("IPv4 Header:\n");
            printf("----------------\n");
            printf("Version:\t\t\t%02x\n", ipHeader.versionLength >> 4);

            // Print internet header length
            uint8_t internetLength = ipHeader.versionLength & 0x0F;
            printf("Internet header length:\t\t%02u\n", internetLength);

            // Print DSCP
            uint8_t DSCP = (ipHeader.DSCPECN >> 2);
            printf("DSCP:\t\t\t\t%02x\n", DSCP);

            // Print ECN
            uint8_t ECN = (ipHeader.DSCPECN & 0x03);
            printf("ECN:\t\t\t\t%02x\t", ECN);

            // Print the type of ECN
            switch (ECN) {
            case 0:
                printf("Non-ECT Packet\n");
                break;
            case 1:
                printf("ECN-Capable Packet\n");
                break;
            case 3:
                printf("Congestion Experienced\n");
                break;
            default:
                printf("Unknown\n");
            }

            // Print the total length
            uint16_t shiftLength = (ipHeader.ipLength >> 8) | (ipHeader.ipLength << 8);
            printf("Total Length:\t\t\t%u\n", shiftLength);

            // Print IP ID
            uint16_t shiftHeader = (ipHeader.ipID >> 8) | (ipHeader.ipID << 8);
            printf("Identification:\t\t\t%u\n", shiftHeader);

            // Print Flags
            printf("Flags:\t\t\t\t");
            uint16_t fragment = (ipHeader.offSetFlagFragment >> 8) | (ipHeader.offSetFlagFragment << 8);
            uint8_t flag = (fragment >> 13);
            if (flag & 0x02) {
                printf("Don't Fragment\n");
            }
            else if (flag & 0x01) {
                printf("More Fragments\n");
            }
            else {
                printf("No Flag Set\n");
            }

            // Print Fragment offset
            printf("Fragment offset:\t\t%u\n", fragment & 0x1FFF);

            // Print Time to Live
            printf("Time to Live:\t\t\t%u\n", ipHeader.liveTime);

            // Print Protcol
            printf("Protocol:\t\t\t%u\n", ipHeader.ipProtocol);

            // Print CheckSum
            uint16_t shiftCheckSum = (ipHeader.checkSum >> 8) | (ipHeader.checkSum << 8);
            printf("IP CheckSum:\t\t\t0x%04x\n", shiftCheckSum);

            // Print source ip Address
            printf("Source IP Address:\t\t%u.%u.%u.%u\n",
                ipHeader.sourceAddress & 0xFF,
                (ipHeader.sourceAddress >> 8) & 0xFF,
                (ipHeader.sourceAddress >> 16) & 0xFF,
                (ipHeader.sourceAddress >> 24) & 0xFF);

            // Print Destination ip address
            printf("Destination IP Address:\t\t%u.%u.%u.%u\n",
                ipHeader.destinationAddress & 0xFF,
                (ipHeader.destinationAddress >> 8) & 0xFF,
                (ipHeader.destinationAddress >> 16) & 0xFF,
                (ipHeader.destinationAddress >> 24) & 0xFF);



            // Print options
            printf("Options:\t\t\t");

            // if there are options print all with the 4 bytes
            if (internetLength > 5) {
                // option counter for indexing starting at 0
                uint8_t index = 0;
                // current byte
                uint8_t byte = 0;
                // length of the options to print *4 for bytes
                uint8_t length = (internetLength - 5) * 4;
                // bytes printed
                int currentOption = 0;
                // boolean (int 1 or 0) for while loop status
                int isTrue = 1;

                // loop for all options
                while (isTrue) {

                    // if all options are printed change status
                    if (currentOption >= length) {
                        isTrue = 0;
                    } // else read in a byte
                    else if (fread(&byte, sizeof(byte), 1, inFile) == 1) {
                        // if not the first and 4 bytes bytes have been printed
                        if (currentOption % 4 == 0) {
                            printf("\nOption %d: \t\t\t %02x", index, byte);
                            index++;
                        }
                        else {
                            // print byte
                            printf(" %02x", byte);
                        }
                    }
                    else {
                        // if no bit can be read in change status
                        isTrue = 0;
                    }

                    // increment the number of bytes printed
                    currentOption++;
                }
            }
            else { // if size == 5 then no options
                printf("No Options\n");
            }

            struct TCPHeader tcpHeader;

            if (fread(&tcpHeader, sizeof(tcpHeader), 1, inFile) == 1) {

                printf("\n\nTCP Header");
                printf("----------------\n");

                // Print the source Port
                uint16_t sPort = (tcpHeader.sourcePort >> 8) | (tcpHeader.sourcePort << 8);
                printf("Source Port:\t\t\t%u\n", sPort);

                // Print the destination port
                uint16_t dPort = (tcpHeader.destinationPort >> 8) | (tcpHeader.destinationPort << 8);
                printf("Destination Port:\t\t%u\n", dPort);

                //Print the sequence Number
                uint32_t sNum = ((tcpHeader.sequenceNumber << 8) & 0xFF00FF00) | ((tcpHeader.sequenceNumber >> 8) & 0xFF00FF);
                sNum = (sNum << 16) | (sNum >> 16);
                printf("Sequence Number:\t\t%u\n", sNum);

                //Print the ack Number
                uint32_t aNum = ((tcpHeader.ackNumber << 8) & 0xFF00FF00) | ((tcpHeader.ackNumber >> 8) & 0xFF00FF);
                aNum = (aNum << 16) | (aNum >> 16);
                printf("Ack Number:\t\t\t%u\n", aNum);

                // Print Data Offset and Flag
                uint16_t offSet = (tcpHeader.dataOffsetFlag >> 8) | (tcpHeader.dataOffsetFlag << 8);
                offSet = offSet >> 12;
                printf("Data Offset:\t\t\t%u\n", offSet);

                // Print Flag
                uint8_t flag = (tcpHeader.dataOffsetFlag >> 8) | (tcpHeader.dataOffsetFlag << 8);
                flag = flag << 8 >> 8;

                int urgCheck = 0;

                printf("Flags:\t\t\t\t");
                if (flag & 0x80) {
                    printf("CWR ");
                }
                if (flag & 0x40) {
                    printf("ECE ");
                }
                if (flag & 0x20) {
                    printf("URG ");
                    urgCheck = 1;
                }
                if (flag & 0x10) {
                    printf("ACK ");
                }
                if (flag & 0x8) {
                    printf("PSH ");
                }
                if (flag & 0x4) {
                    printf("RST ");
                }
                if (flag & 0x2) {
                    printf("SYN ");
                }
                if (flag & 0x1) {
                    printf("FIN ");
                }

                // Print Window Size
                uint16_t wSize = (tcpHeader.windowSize >> 8) | (tcpHeader.windowSize << 8);
                printf("\nWindow Size:\t\t\t%u\n", wSize);

                // Print TCP Check Sum
                uint16_t tcpCheckSum  = (tcpHeader.checkSum >> 8) | (tcpHeader.checkSum << 8);
                printf("IP CheckSum:\t\t\t0x%04x\n", tcpCheckSum);
                
                // Print Urgent Pointers
                uint16_t urgPointers = (tcpHeader.urgPointer >> 8) | (tcpHeader.urgPointer << 8);

                // if URG was tripped define the offset
                if (urgCheck = 1) {
                    printf("Urgent Pointer:\t\t\t%u\n", sNum & urgPointers);
                }
                else {
                    printf("Urgent Pointer:\t\t\t%u\n", urgPointers);
                }
                

                // Print options
                printf("Options:\t\t\t");

                // if there are options print all with the 4 bytes
                if (offSet > 5) {
                    // option counter for indexing starting at 0
                    uint8_t index = 0;
                    // current byte
                    uint8_t byte = 0;
                    // length of the options to print *4 for bytes
                    uint8_t length = (offSet - 5) * 4;
                    // bytes printed
                    int currentOption = 0;
                    // boolean (int 1 or 0) for while loop status
                    int isTrue = 1;

                    // loop for all options
                    while (isTrue) {

                        // if all options are printed change status
                        if (currentOption >= length) {
                            isTrue = 0;
                        } // else read in a byte
                        else if (fread(&byte, sizeof(byte), 1, inFile) == 1) {
                            // if not the first and 4 bytes bytes have been printed
                            if (currentOption % 4 == 0) {
                                printf("\nOption %d: \t\t\t %02x", index, byte);
                                index++;
                            }
                            else {
                                // print byte
                                printf(" %02x", byte);
                            }
                        }
                        else {
                            // if no bit can be read in change status
                            isTrue = 0;
                        }

                        // increment the number of bytes printed
                        currentOption++;
                    }
                }
                else { // if size == 5 then no options
                    printf("No Options\n");
                }


                // Print Header for payload
                printf("\n\nPayload:\n");

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
}
