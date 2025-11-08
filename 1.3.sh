#!/bin/bash

# --- Configuration: Persistent File ---
addressBook='address_book_v2.txt'
DELIMITER=":" # Define the colon as the delimiter
touch "$addressBook"

echo "Using persistent address book file: $addressBook"

# --- Utility Function: Display all Records ---
displayRecords() {
    echo -e "\n=============================================="
    echo " ALL CONTACTS"
    echo "=============================================="
    if [[ ! -s "$addressBook" ]]; then
        echo "The address book is currently empty."
        echo "----------------------------------------------"
        return
    fi
    echo -e "Name\t\tPhone\t\tEmail"
    echo "----------------------------------------------"
    # Use column for neat display, specifying the colon delimiter
    cat "$addressBook" | column -t -s "$DELIMITER"
    echo "----------------------------------------------"
}

# --- Menu ---
menu() {
    echo -e "\n=============================="
    echo " Address Book Menu (v2)"
    echo "=============================="
    echo "1. Search address book"
    echo "2. Add new entry"
    echo "3. Modify entries (Remove / Edit)"
    echo "4. Display all records"
    echo "5. Quit the program"
    echo "------------------------------"
}

# --- Function 1: Search Address Book ---
searchAddressBook() {
    echo "--- Search Contacts ---"
    if [[ ! -s "$addressBook" ]]; then
        echo "The address book is empty."
        return
    fi

    read -r -p "Enter search term (Name, Phone, or Email): " term

    # Use grep to find matches for the term (case-insensitive, print line number)
    results=$(grep -i -n "$term" "$addressBook")

    if [[ -z "$results" ]]; then
        echo -e "\nNo contacts found matching '$term'."
    else
        echo -e "\n--- Search Results for '$term' ---"
        echo -e "Line #\tName\t\tPhone\t\tEmail"
        echo "------------------------------------------------"
        # Display results neatly: first cut the line number (f1), then pipe to column
        echo "$results" | cut -d: -f2- | column -t -s "$DELIMITER" | awk -F'\t' '{ print NR"\t"$0 }'
    fi
}

# --- Function 2: Add Entry ---
addEntry() {
    echo "--- Add New Entry ---"

    read -r -p "Name: " name
    read -r -p "Phone Number: " phone
    read -r -p "Email Address: " email

    # Basic input validation
    if [[ -z "$name" || -z "$phone" || -z "$email" ]]; then
        echo "All fields are required. Entry aborted."
        return
    fi

    # Format the new entry: Name:Phone:Email
    echo "$name$DELIMITER$phone$DELIMITER$email" >> "$addressBook"
    echo -e "\nContact Added: $name ($phone)"
}

# --- Function 3 Handler: Remove / Edit Entries ---
modifyEntry() {
    echo "--- Modify Entry ---"

    if [[ ! -s "$addressBook" ]]; then
        echo "The address book is empty."
        return
    fi
    
    # Display the options
    echo "A) Remove an entry"
    echo "B) Edit an entry"
    read -r -p "Choose action (A/B) or press Enter to cancel: " action

    case "$action" in
        [Aa]) removeEntry ;;
        [Bb]) editEntry ;;
        *) echo "Modification cancelled." ;;
    esac
}

# --- Sub-Function: Remove an entry ---
removeEntry() {
    read -r -p "Enter the FULL NAME (e.g., 'John Doe') of the contact to REMOVE: " name_to_remove

    # Check if the entry exists
    if grep -q -i "$name_to_remove" "$addressBook"; then
        # Use sed -i (in-place) with -e to delete lines containing the name.
        # We use '^' and ':$' in the regex to roughly match the whole record.
        sed -i -e "/^$name_to_remove:/d" "$addressBook"
        echo -e "\nContact(s) matching '$name_to_remove' successfully removed."
    else
        echo -e "\nError: Contact matching '$name_to_remove' not found."
    fi
}

# --- Sub-Function: Edit an entry (simple replace by name) ---
editEntry() {
    read -r -p "Enter the FULL NAME of the contact to EDIT: " old_name

    # Check if entry exists
    if ! grep -q -i "^$old_name$DELIMITER" "$addressBook"; then
        echo -e "\nError: Contact matching '$old_name' not found."
        return
    fi
    
    # Prompt for new details
    echo -e "\nEnter the NEW details for $old_name:"
    read -r -p "New Name: " new_name
    read -r -p "New Phone Number: " new_phone
    read -r -p "New Email Address: " new_email
    
    # Input validation for new fields
    if [[ -z "$new_name" || -z "$new_phone" || -z "$new_email" ]]; then
        echo "All new fields are required. Edit aborted."
        return
    fi
    
    # Create the new record
    local new_record="$new_name$DELIMITER$new_phone$DELIMITER$new_email"

    # Use sed -i (in-place) to find the old record line and substitute it with the new record.
    # We use a temp file for portability with older sed versions.
    sed -i -e "/^$old_name$DELIMITER/ c\\
$new_record" "$addressBook"
    
    echo -e "\nContact '$old_name' successfully updated to '$new_name'."
}


# --- Main loop ---
main_loop() {
    while true; do
        menu
        read -r -p "Choose an option (1-5): " choice

        case "$choice" in
            1) searchAddressBook ;;
            2) addEntry ;;
            3) modifyEntry ;;
            4) displayRecords ;;
            5) echo "Exiting program. Goodbye!"; break ;;
            *) echo "Invalid choice. Please enter a number between 1 and 5." ;;
        esac
    done
}

# Start the application
main_loop