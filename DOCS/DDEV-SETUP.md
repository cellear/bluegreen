# DDEV Setup Guide for Blue-Green Deployment Module

This guide shows you how to set up a complete DDEV environment for Backdrop CMS with the Blue-Green Deployment module, including automatic dual-domain support.

## Automated Setup (Recommended)

The easiest way to get started is using our automated setup script:

### Quick Start

```bash
# 1. Create a new directory for your project
mkdir my-backdrop-site
cd my-backdrop-site

# 2. Download and run the setup script
curl -O https://raw.githubusercontent.com/cellear/bluegreen/claude/backd-exploration-011CUp2W9odxYjCQKunNsHgn/setup-ddev.sh
chmod +x setup-ddev.sh
./setup-ddev.sh my-project-name

# Or run it from the module after cloning:
# git clone https://github.com/cellear/bluegreen.git
# ./bluegreen/setup-ddev.sh my-project-name
```

### What the Script Does

The `setup-ddev.sh` script automatically:

1. **Configures DDEV** for Backdrop CMS with PHP 8.1
2. **Sets up dual domains**:
   - Main domain: `{project-name}.ddev.site` (routes to active environment)
   - Dev domain: `dev.{project-name}.ddev.site` (routes to idle environment)
3. **Starts DDEV** and displays next steps

### After Running the Script

Follow the displayed next steps to:

1. **Install Backdrop CMS**:
   ```bash
   ddev composer create backdrop/backdrop
   ddev launch
   ```
   Complete the Backdrop installation wizard in your browser.

2. **Install the Blue-Green module**:
   ```bash
   mkdir -p modules && cd modules
   git clone https://github.com/cellear/bluegreen.git
   cd bluegreen
   git checkout claude/backd-exploration-011CUp2W9odxYjCQKunNsHgn
   cd ../..
   ```

3. **Enable the module**: Visit `admin/modules`, enable "Blue-Green Deployment"

4. **Run the setup wizard**: Visit `admin/content/bluegreen` and click the Setup tab

## Manual Setup

If you prefer to set things up manually or need to understand the process:

### 1. Configure DDEV

```bash
cd /path/to/your/backdrop/site
ddev config --project-type=backdrop --project-name=YOUR-PROJECT-NAME --php-version=8.1
```

### 2. Add Dual Domain Support

Edit `.ddev/config.yaml` and add:

```yaml
additional_fqdns:
  - dev.YOUR-PROJECT-NAME.ddev.site
```

Or append it automatically:

```bash
echo -e "\nadditional_fqdns:\n  - dev.YOUR-PROJECT-NAME.ddev.site" >> .ddev/config.yaml
```

### 3. Start DDEV

```bash
ddev start
```

### 4. Install Backdrop and the Module

Follow steps 1-4 from "After Running the Script" above.

## Domain Configuration

Once everything is installed and the Blue-Green setup wizard completes:

- **Main domain** (`your-project.ddev.site`): Always shows the **active/live** environment
- **Dev domain** (`dev.your-project.ddev.site`): Always shows the **idle/dev** environment

This is automatically handled by `settings.bluegreen.php` which detects the domain and routes to the appropriate database.

## Example Workflow

```bash
# 1. Visit main domain - you're on Blue (active)
open https://my-site.ddev.site

# 2. Visit dev domain - you're on Green (idle)
open https://dev.my-site.ddev.site

# 3. Make changes on dev domain (Green environment)
# Test your changes thoroughly

# 4. Switch environments via admin/content/bluegreen
# Click "Make Green Live"

# 5. Now the domains have swapped:
#    - Main domain → Green (now active)
#    - Dev domain → Blue (now idle)
```

## Troubleshooting

### Script fails with "command not found"

Make sure DDEV is installed and in your PATH:
```bash
ddev version
```

### Domains don't resolve

```bash
# Restart DDEV to update hosts file
ddev restart

# Verify domains resolve
ping your-project.ddev.site
ping dev.your-project.ddev.site
```

### Wrong environment shows on domain

1. Check which environment is active: Visit `admin/content/bluegreen`
2. Clear Backdrop cache: `ddev drush cc all`
3. Check `settings.bluegreen.php` exists in site root

## More Information

- [Multi-Domain Configuration Details](DDEV-MULTI-DOMAIN.md) - In-depth technical documentation
- [Main README](../README.md) - Module features and usage
- [Dual Domain Setup](DUAL-DOMAIN-SETUP.md) - Architecture and configuration details

## Support

For issues or questions:
- Check the [GitHub repository](https://github.com/cellear/bluegreen)
- Review existing issues or create a new one
