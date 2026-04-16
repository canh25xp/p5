#!/bin/bash

# Script to generate documentation for all p4 commands
# Usage: ./generate_p4_docs.sh

DOCS_DIR="docs/p4/commands"
COMMANDS_FILE="docs/p4/commands.md"

# Create docs directory if it doesn't exist
mkdir -p "$DOCS_DIR"

# Extract command names from the commands.md file
echo "Extracting p4 commands..."
COMMANDS=$(grep -E '^- [a-zA-Z0-9]' "$COMMANDS_FILE" | awk '{print $2}' | sort | uniq)

echo "Found $(echo "$COMMANDS" | wc -l) commands"

# Process each command
for cmd in $COMMANDS; do
    # Skip if file already exists
    if [ -f "$DOCS_DIR/$cmd.md" ]; then
        echo "Skipping $cmd (already exists)"
        continue
    fi

    echo "Processing $cmd..."

    # Get help output
    HELP_OUTPUT=$(p4 help "$cmd" 2>&1)

    # Check if command exists
    if [[ $HELP_OUTPUT == *"Unknown command"* ]] || [[ $HELP_OUTPUT == *"invalid command"* ]]; then
        echo "  Warning: Unknown command '$cmd'"
        continue
    fi

    # Create markdown file
    {
        echo "# p4 $cmd"
        echo ""
        echo "$HELP_OUTPUT"
    } >"$DOCS_DIR/$cmd.md"

    echo "  Created $DOCS_DIR/$cmd.md"
done

echo "Documentation generation complete!"

