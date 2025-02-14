#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// Convert string to lowercase for condition checking
string toLowerCase(const string& str) {
    string lower;
    for (char c : str) {
        lower += tolower(c);
    }
    return lower;
}

// Structure to hold the information of each item
struct Item {
    string category;
    string name;
    int price;
    int quantity;
};

// Structure to hold purchased items
struct PurchasedItem {
    string name;
    int price;
    int quantity;
};

// Load the CSV file into a vector of items
vector<Item> loadItemsFromCSV(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    string line, word;

    // Skip the header line
    getline(file, line);

    // Reading each line
    while (getline(file, line)) {
        Item item;
        stringstream s(line);

        // Read each column of the CSV
        getline(s, item.category, ',');
        getline(s, item.name, ',');
        getline(s, word, ',');
        item.price = stoi(word);
        getline(s, word, ',');
        item.quantity = stoi(word);

        items.push_back(item);
    }

    return items;
}

// Update the CSV file after making changes to the items
void updateCSV(const string& filename, const vector<Item>& items) {
    ofstream file(filename);
    file << "Category,Name,Price,Quantity\n";  // Write the header

    for (const auto& item : items) {
        file << item.category << "," << item.name << "," << item.price << "," << item.quantity << "\n";
    }
    file.close();
}

// Function to handle the purchase of an item
void PurchaseItem(vector<Item>& items, vector<PurchasedItem>& purchasedItems, const string& category, string choice) {
    bool itemFound = false;
    int quantity;

    for (auto& item : items) {  
        if (toLowerCase(item.name) == toLowerCase(choice) && toLowerCase(item.category) == toLowerCase(category)) {  
            itemFound = true;

            if (item.quantity == 0) {
                cout << "Sorry, " << item.name << " is out of stock!" << endl;
                return;  // Return control to main for the shopping prompt
            }

            cout << "Enter the number of units you want to purchase: ";
            cin >> quantity;
            cin.ignore();  // Clear newline character after quantity input

            // Check if the requested quantity is available
            if (quantity > item.quantity) {
                cout << "Only " << item.quantity << " units of " << item.name << " are available." << endl;
                do {
                    cout << "Please re-enter the number of units you want to purchase: ";
                    cin >> quantity;
                    cin.ignore();
                } while (quantity > item.quantity);
            }

            // Reduce the stock by the purchased amount
            item.quantity -= quantity;

            // Add to purchased items list
            purchasedItems.push_back({item.name, item.price, quantity});

            cout << "You have purchased " << quantity << " unit(s) of " << item.name << "." << endl;

            // After purchasing, update the file
            updateCSV("items_list.csv", items);

            return;  // Return control to main for the shopping prompt
        }
    }

    // If the item was not found in the category
    if (!itemFound) {
        cout << "Item not available!" << endl;
        return;  // Return control to main for the shopping prompt
    }
}

// Function to print items for a given category and handle purchases
void CheckCategory(vector<Item>& items, vector<PurchasedItem>& purchasedItems, const string& category) {
    bool categoryFound = false;
    bool itemsAvailable = false;
    string choice;

	
    // Print available items in the category (with quantity > 0)
    for (const auto& item : items) {
        if (toLowerCase(item.category) == toLowerCase(category) && item.quantity > 0) {
            categoryFound = true;
            itemsAvailable = true;
            cout << item.name << " : Rs." << item.price << endl;
        }
    }

    if (categoryFound && itemsAvailable) {
        cout << "Please select an item: ";
        getline(cin, choice);
        PurchaseItem(items, purchasedItems, category, choice);  // Pass the category and selected item
    } 
    else if (!categoryFound) {
        cout << "Category not found!" << endl;
        return;  // Return control to main for the shopping prompt
    } 
    else if (!itemsAvailable) {
        cout << "No items available in this category." << endl;
        return;  // Return control to main for the shopping prompt
    }
}

// Function to remove an item from the purchased list
void RemoveItem(vector<Item>& items, vector<PurchasedItem>& purchasedItems) {
    string choice;
    cout << "Would you like to remove any items from your purchase (Yes/No)? ";
    getline(cin, choice);

    if (toLowerCase(choice) == "yes") {
        string removeItem;
        int removeQty;
        cout << "Enter the name of the item to remove: ";
        getline(cin, removeItem);

        bool found = false;
        for (auto& pItem : purchasedItems) {
            if (toLowerCase(pItem.name) == toLowerCase(removeItem)) {
                found = true;
                cout << "Enter the quantity to remove: ";
                cin >> removeQty;
                cin.ignore();

                if (removeQty > pItem.quantity) {
                    cout << "You only purchased " << pItem.quantity << " units. Removing all units." << endl;
                    removeQty = pItem.quantity;
                }

                // Update the item's quantity in stock
                for (auto& item : items) {
                    if (toLowerCase(item.name) == toLowerCase(pItem.name)) {
                        item.quantity += removeQty;
                    }
                }

                // Update or remove the item from the purchased list
                if (removeQty == pItem.quantity) {
                    pItem.quantity = 0;  // Mark for removal
                } else {
                    pItem.quantity -= removeQty;
                }

                // Update the CSV after changes
                updateCSV("items_list.csv", items);
                break;
            }
        }

        if (!found) {
            cout << "Item not found in your purchases." << endl;
        }

        // Clean up the purchasedItems vector (remove items with 0 quantity)
        purchasedItems.erase(
            remove_if(purchasedItems.begin(), purchasedItems.end(), [](const PurchasedItem& p) { return p.quantity == 0; }),
            purchasedItems.end()
        );
    }
}


// Function to handle billing and print the final bill
void Billing(vector<Item>& items, vector<PurchasedItem>& purchasedItems) {
    cout << "\n\n------------ Proceeding to Billing ------------\n";
    float totalAmount = 0;
    const float taxRate = 0.05;  // 5% tax

    // Print header
    cout << left << setw(21) << "Item" << setw(20) << "Quantity" << setw(25) << "Price" << endl;
    cout << "-----------------------------------------------" << endl;

    // Calculate the total amount and print the bill
    for (const auto& pItem : purchasedItems) {
        float itemTotal = pItem.price * pItem.quantity;
        cout << left << setw(21) << pItem.name << setw(20) << pItem.quantity << setw(25) << itemTotal << endl;
        totalAmount += itemTotal;
    }

    // Apply tax
    float taxAmount = totalAmount * taxRate;
    totalAmount += taxAmount;

    // Print the total and tax
    cout << "-----------------------------------------------" << endl;
    cout << left << setw(21) << "Tax (5%)" << setw(20) << " " << setw(25) << taxAmount << endl;
    cout << "Total" << setw(36) << " " << totalAmount << endl;

    
    
}

// Main function for the shopping program
int main() {
    string choice;
    string category;
    vector<Item> items = loadItemsFromCSV("items_list.csv");
    vector<PurchasedItem> purchasedItems;

    do {
        // Display categories
        cout << "Dairy\nGrocery\nAppliances\nMobiles\nElectronics\nFurniture\nCosmetics\nAuto-Accessories\n";
        cout << "\nPlease Select a category: ";
        getline(cin, category);

        // Call function to check and print items for the entered category
        CheckCategory(items, purchasedItems, category);

        // Continue shopping prompt
        cout << "Continue Shopping (Yes/No)? ";
        getline(cin, choice);
        if (toLowerCase(choice) == "no") {
        	RemoveItem(items, purchasedItems);
            Billing(items, purchasedItems);  // Proceed to billing
            cout << "\n-------- Thank You for shopping with us -------\n";
            break;
        } else if (toLowerCase(choice) != "yes" && toLowerCase(choice) != "no") {
            do {
                cout << "\nPlease enter a valid choice: ";
                getline(cin, choice);
            } while (toLowerCase(choice) != "no" && toLowerCase(choice) != "yes");
        }
    } while (true);

    return 0;
}
