#!/bin/bash

# --- Configuration: Persistent File ---
# The address book data is stored in 'address.txt' and persists between runs.
addBk='address.txt'
touch "$addBk"
FILE_NAME="$addBk"
DELIMITER=";"

echo "Using persistent address book file: $addBk"

# --- Utility Function ---
# Generates the next available sequential ID.
getNextId() {
    # If the file is empty, start at 1.
    if [[ ! -s "$addBk" ]]; then
        echo 1
        return
    fi
    # Find the largest ID (first field), sort numerically, take the last one, and add 1.
    cut -d "$DELIMITER" -f 1 "$addBk" | sort -n | tail -1 | awk '{print $1 + 1}'
}

# --- Menu ---
menu() {
	echo -e "\n=============================="
    echo " Address Book Menu"
    echo "=============================="
	echo "1. Search Address Book"
	echo "2. Add an address book entry"
	echo "3. Remove an address book entry (by ID)"
	echo "4. Quit the program"
    echo "------------------------------"
}

# --- Function 1: Search Address Book ---
searchAddressBook() {
    echo "--- Search Contacts ---"
    if [[ ! -s "$addBk" ]]; then
        echo "The address book is empty."
        return
    fi

    read -r -p "Enter search term (Name, Phone, or ID): " term

    # Use grep to find matches for the term
    results=$(grep -i "$term" "$addBk")

    if [[ -z "$results" ]]; then
        echo -e "\nNo contacts found matching '$term'."
    else
        echo -e "\nID\tName\t\tPhone"
        echo "-------------------------------------"
        # Use column to display results neatly
        echo "$results" | column -t -s "$DELIMITER"
    fi
}

# --- Function 2: Add an address book entry ---
addEntry() {
    echo "--- Add New Entry ---"

    read -r -p "Name: " name
    read -r -p "Phone No.: " phone

    # Basic input validation
    if [[ -z "$name" || -z "$phone" ]]; then
        echo "Name and Phone cannot be empty. Entry aborted."
        return
    fi

    # Generate the next ID
    local next_id=$(getNextId)

    # Format the new entry: ID;Name;Phone
    echo "$next_id$DELIMITER$name$DELIMITER$phone" >> "$addBk"
    echo -e "\nContact Added: ID $next_id, Name $name"
}

# --- Function 3: Remove an address book entry ---
removeEntry() {
    echo "--- Remove Entry ---"

    if [[ ! -s "$addBk" ]]; then
        echo "The address book is empty."
        return
    fi

    read -r -p "Enter ID of the contact to remove: " id_to_remove

    # Check if ID exists (ID is the first field, so we look for 'ID;')
    if grep -q "^$id_to_remove$DELIMITER" "$addBk"; then
        # Use sed to delete the line starting with the ID and the delimiter
        sed -i "/^$id_to_remove$DELIMITER/d" "$addBk"
        echo -e "\nContact with ID $id_to_remove successfully removed."
    else
        echo -e "\nError: Contact with ID $id_to_remove not found."
    fi
}

# --- Main loop ---
main_loop() {
    while true; do
	    menu
	    read -r -p "Choose an option (1-4): " choice

	    case "$choice" in
            1) searchAddressBook ;;
            2) addEntry ;;
            3) removeEntry ;;
            4) echo "Exiting program. Goodbye!"; break ;;
	        *) echo "Invalid choice. Please enter a number between 1 and 4." ;;
	    esac
    done
}

# Start the application
main_loop