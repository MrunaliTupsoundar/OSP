#!/bin/bash

# --- Configuration ---
# Set the current directory for file operations
CURRENT_DIR=$(pwd)

# --- Menu ---
menu() {
    echo -e "\n=============================="
    echo " File Management Menu"
    echo " (Current Directory: $CURRENT_DIR)"
    echo "=============================="
    echo "1. Test if file exists"
    echo "2. Read a file (Display contents)"
    echo "3. Delete a file"
    echo "4. Display a list of files"
    echo "5. Quit the program"
    echo "------------------------------"
}

# --- Function 1: Test if file exists ---
testFileExists() {
    echo "--- File Existence Check ---"
    read -r -p "Enter filename to check: " filename

    # Check if the file is a regular file (-f) and exists
    if [[ -f "$filename" ]]; then
        echo -e "\nSUCCESS: The file '$filename' exists."
    elif [[ -d "$filename" ]]; then
        echo -e "\nINFO: '$filename' exists, but it is a directory, not a regular file."
    else
        echo -e "\nFAIL: The file or path '$filename' was not found."
    fi
}

# --- Function 2: Read a file (Display contents) ---
readFileContent() {
    echo "--- Read File Contents ---"
    read -r -p "Enter filename to read: " filename

    if [[ -f "$filename" ]]; then
        echo -e "\n--- Contents of '$filename' ---"
        cat "$filename"
        echo "------------------------------"
    else
        echo -e "\nError: '$filename' is not a file or does not exist."
    fi
}

# --- Function 3: Delete a file ---
deleteFile() {
    echo "--- Delete File ---"
    read -r -p "Enter filename to delete: " filename

    if [[ -f "$filename" ]]; then
        echo "WARNING: This action is permanent."
        read -r -p "Are you sure you want to delete '$filename'? (y/N): " confirmation
        
        # Check for confirmation
        if [[ "$confirmation" =~ ^[Yy]$ ]]; then
            # Attempt to delete the file
            if rm "$filename"; then
                echo -e "\nSUCCESS: File '$filename' has been deleted."
            else
                echo -e "\nError: Could not delete '$filename'. Check permissions."
            fi
        else
            echo -e "\nDeletion cancelled by user."
        fi
    else
        echo -e "\nError: '$filename' is not a file or does not exist. Cannot delete."
    fi
}

# --- Function 4: Display a list of files ---
displayFileList() {
    echo -e "\n--- Files in $CURRENT_DIR ---"
    
    # Use 'ls -lA' to list files, including hidden ones (but excluding . and ..) 
    # and provide detailed information.
    ls -lA 
}

# --- Main loop ---
main_loop() {
    while true; do
        menu
        read -r -p "Choose an option (1-5): " choice

        case "$choice" in
            1) testFileExists ;;
            2) readFileContent ;;
            3) deleteFile ;;
            4) displayFileList ;;
            5) echo "Exiting program. Goodbye!"; break ;;
            *) echo "Invalid choice. Please enter a number between 1 and 5." ;;
        esac
    done
}

# Start the application
main_loop