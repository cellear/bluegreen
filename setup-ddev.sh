#!/bin/bash
#
# DDEV Setup Script for Blue-Green Deployment Module
#
# This script automates the setup of a DDEV environment for Backdrop CMS
# with dual-domain support for the Blue-Green deployment module.
#

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get project name from argument or prompt
if [ -z "$1" ]; then
    echo -e "${BLUE}Enter your DDEV project name (e.g., bluegreen-v4):${NC}"
    read -r PROJECT_NAME
else
    PROJECT_NAME="$1"
fi

if [ -z "$PROJECT_NAME" ]; then
    echo "Error: Project name cannot be empty"
    exit 1
fi

echo -e "${GREEN}Setting up DDEV for Backdrop CMS with project name: ${PROJECT_NAME}${NC}"

# Check if we're in the right directory
if [ -f "ddev-config-snippet.yaml" ]; then
    echo -e "${YELLOW}Warning: It looks like you're running this from inside the bluegreen module directory.${NC}"
    echo -e "${YELLOW}This script should be run from your Backdrop site root.${NC}"
    echo -e "${YELLOW}Continuing anyway...${NC}"
fi

# Step 1: Configure DDEV
echo -e "\n${BLUE}Step 1: Configuring DDEV...${NC}"
ddev config --project-type=backdrop --project-name="${PROJECT_NAME}" --php-version=8.1

# Step 2: Add dual domain support
echo -e "\n${BLUE}Step 2: Adding dual domain configuration...${NC}"

DEV_DOMAIN="dev.${PROJECT_NAME}.ddev.site"

# Check if additional_fqdns already exists in config
if grep -q "^additional_fqdns:" .ddev/config.yaml 2>/dev/null; then
  # Check if our dev domain is already in the list
  if grep -q "$DEV_DOMAIN" .ddev/config.yaml; then
    echo -e "${GREEN}✓ Dev domain already configured: $DEV_DOMAIN${NC}"
  else
    # Add to existing additional_fqdns list
    sed -i.bak "/^additional_fqdns:/a\\
  - $DEV_DOMAIN" .ddev/config.yaml
    echo -e "${GREEN}✓ Added $DEV_DOMAIN to existing additional_fqdns${NC}"
  fi
else
  # No additional_fqdns section exists, create it
  cat >> .ddev/config.yaml <<EOF

# Blue-Green Deployment Module: Dual Domain Support
# Main domain: ${PROJECT_NAME}.ddev.site (active/live environment)
# Dev domain: dev.${PROJECT_NAME}.ddev.site (inactive/dev environment)
additional_fqdns:
  - $DEV_DOMAIN
EOF
  echo -e "${GREEN}✓ Created additional_fqdns with $DEV_DOMAIN${NC}"
fi

# Step 3: Start DDEV
echo -e "\n${BLUE}Step 3: Starting DDEV...${NC}"
ddev start

# Step 4: Display next steps
echo -e "\n${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}DDEV environment setup complete!${NC}"
echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "\n${BLUE}Your domains:${NC}"
echo -e "  Main (live):  https://${PROJECT_NAME}.ddev.site"
echo -e "  Dev (idle):   https://dev.${PROJECT_NAME}.ddev.site"
echo -e "\n${BLUE}Next steps:${NC}"
echo -e "  1. Install Backdrop CMS:"
echo -e "     ${YELLOW}ddev composer create backdrop/backdrop${NC}"
echo -e "     ${YELLOW}ddev launch${NC} (follow the installation wizard)"
echo -e "\n  2. Install the Blue-Green Deployment module:"
echo -e "     ${YELLOW}mkdir -p modules && cd modules${NC}"
echo -e "     ${YELLOW}git clone https://github.com/cellear/bluegreen.git${NC}"
echo -e "     ${YELLOW}cd bluegreen && git checkout claude/backd-exploration-011CUp2W9odxYjCQKunNsHgn${NC}"
echo -e "     ${YELLOW}cd ../..${NC}"
echo -e "\n  3. Enable the module at: admin/modules"
echo -e "\n  4. Run the setup wizard at: ${YELLOW}admin/content/bluegreen${NC}"
echo -e "\n${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
