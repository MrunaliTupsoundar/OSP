#!/bin/bash

# --- Configuration: Persistent File ---
phoneBook='phonebook.txt'
DELIMITER=$'\t' # Define TAB as the delimiter
touch "$phoneBook"

echo "Using persistent phone book file: $phoneBook"

# --- Utility Function: Display Phone Book Content ---
displayBook() {
    echo -e "\n-------------------------------------"
    if [[ ! -s "$phoneBook" ]]; then
        echo "The phone book is empty."
        echo "-------------------------------------"
        return
    fi
    echo -e "First Name\tLast Name\tPhone Number"
    echo "-------------------------------------"
    # Use column for neat display, using the Tab delimiter
    cat "$phoneBook" | column -t -s "$DELIMITER"
    echo "-------------------------------------"
}

# --- Menu ---
menu() {
    echo -e "\n=============================="
    echo " Phone Book Menu"
    echo "=============================="
    echo "1. Add a new entry"
    echo "2. Search entries (Name or Phone)"
    echo "3. Sort phone book by Last Name"
    echo "4. Delete an entry"
    echo "5. Display all entries"
    echo "6. Quit the program"
    echo "------------------------------"
}

# --- Function 1: Add a new name to a phone book file ---
addEntry() {
    echo "--- Add New Entry ---"

    read -r -p "First Name: " firstName
    read -r -p "Last Name: " lastName
    read -r -p "Phone Number: " phone

    # Basic input validation
    if [[ -z "$firstName" || -z "$lastName" || -z "$phone" ]]; then
        echo "All fields (First Name, Last Name, Phone) are required. Entry aborted."
        return
    fi

    # Format the new entry: FirstName\tLastName\tPhone
    echo -e "$firstName$DELIMITER$lastName$DELIMITER$phone" >> "$phoneBook"
    echo -e "\nContact Added: $firstName $lastName ($phone)"
}

# --- Function 2: Display all matches to a name or phone number (Search) ---
searchEntry() {
    echo "--- Search Contacts ---"
    if [[ ! -s "$phoneBook" ]]; then
        echo "The phone book is empty."
        return
    fi

    read -r -p "Enter search term (Name or Phone): " term

    # Use grep to find matches for the term (case-insensitive)
    results=$(grep -i "$term" "$phoneBook")

    if [[ -z "$results" ]]; then
        echo -e "\nNo contacts found matching '$term'."
    else
        echo -e "\n--- Search Results for '$term' ---"
        echo -e "First Name\tLast Name\tPhone Number"
        echo "-------------------------------------"
        # Display results neatly using the tab delimiter
        echo "$results" | column -t -s "$DELIMITER"
    fi
}

# --- Function 3: Sort the phone book by the last name ---
sortBook() {
    echo "--- Sort Phone Book by Last Name ---"
    if [[ ! -s "$phoneBook" ]]; then
        echo "The phone book is empty. Nothing to sort."
        return
    fi

    # Use 'sort':
    # -t $'\t' : set the delimiter to a tab
    # -k2,2    : sort based only on the second field (Last Name)
    
    sortedResults=$(sort -t "$DELIMITER" -k2,2 "$phoneBook")
    
    echo -e "\n--- Sorted Phone Book ---"
    echo -e "First Name\tLast Name\tPhone Number"
    echo "-------------------------------------"
    # Display sorted results neatly using the tab delimiter
    echo "$sortedResults" | column -t -s "$DELIMITER"
}


# --- Function 4: Delete an entry ---
deleteEntry() {
    echo "--- Delete Entry ---"

    if [[ ! -s "$phoneBook" ]]; then
        echo "The phone book is empty."
        return
    fi
    
    # 1. Display all entries so the user can see what to delete
    displayBook

    read -r -p "Enter the FULL NAME (First and Last, e.g., 'John Doe') of the contact to delete: " full_name_to_remove

    # Check if the entry exists
    if grep -q -i "$full_name_to_remove" "$phoneBook"; then
        # Create a temporary file
        tempFile=$(mktemp)
        
        # Use grep -i -v (case-insensitive, invert match) to copy all lines
        # that DO NOT contain the full name into the temp file.
        grep -i -v "$full_name_to_remove" "$phoneBook" > "$tempFile"
        
        # Overwrite the original phone book with the contents of the temp file
        mv "$tempFile" "$phoneBook"
        
        echo -e "\nAll contacts matching '$full_name_to_remove' successfully removed."
    else
        echo -e "\nError: Contact matching '$full_name_to_remove' not found. Check spelling."
    fi
}

# --- Main loop ---
main_loop() {
    while true; do
        menu
        read -r -p "Choose an option (1-6): " choice

        case "$choice" in
            1) addEntry ;;
            2) searchEntry ;;
            3) sortBook ;;
            4) deleteEntry ;;
            5) displayBook ;;
            6) echo "Exiting program. Goodbye!"; break ;;
            *) echo "Invalid choice. Please enter a number between 1 and 6." ;;
        esac
    done
}

# Start the application
main_loop