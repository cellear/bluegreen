# Blue-Green Deployment Module

Provides blue-green deployment functionality for Backdrop CMS, enabling easy site updates with instant rollback capability.

## Features

- **Dual Database Environments**: Manage separate Blue and Green database environments using efficient 2-table architecture
- **One-Click Switching**: Switch between environments with a single button click
- **Database Sync**: Sync database content from live to dev environment
- **Visual Interface**: Clean, intuitive admin UI showing environment status
- **Backup Support**: Create backups before switching environments
- **Safe Deployment**: Make changes on idle environment, test, then switch live

## Architecture

The module uses a simple 2-table architecture with shared tables:

### Environment Tables
- **Blue environment**: Uses unprefixed tables (the original installation)
- **Green environment**: Uses `alt_` prefixed tables (duplicate for testing)

### Shared Tables
These tables are NOT duplicated and remain shared across both environments:
- `state` - System configuration and tracking
- `watchdog` - Log entries (single audit trail)
- `users` - User accounts (prevents password rollbacks)
- `users_roles`, `role`, `role_permission` - User permissions
- `authmap` - External authentication
- `sessions` - Active sessions (keeps users logged in)

This design minimizes database size while ensuring critical data persists across environment switches.

## Requirements

- Backdrop CMS 1.x installation
- Write permissions on the site's root directory (for creating `settings.bluegreen.php`)

## Installation

### Quick Start with DDEV (Recommended)

For new projects, use our automated DDEV setup script:

```bash
# Run the setup script with your project name
./bluegreen/setup-ddev.sh my-project-name
```

See **[DOCS/DDEV-SETUP.md](DOCS/DDEV-SETUP.md)** for complete DDEV installation guide with dual-domain support.

### Manual Installation

1. Copy the module to `modules/bluegreen/`
2. Enable the module at `admin/modules`
3. Grant permissions at `admin/people/permissions` (search for "blue-green")
4. Visit `admin/content/bluegreen` and run the Setup wizard
5. The wizard will automatically create duplicate tables and configure your environments

## Configuration

The module automatically creates and manages `settings.bluegreen.php` in your site root. This file contains the database configurations for both environments and is automatically included by `settings.php`.

### Automatic Configuration

The Setup wizard automatically creates `settings.bluegreen.php` with the following structure:

```php
// Blue environment (unprefixed tables)
$databases['blue']['default'] = $base_config;
$databases['blue']['default']['prefix'] = '';

// Green environment (alt_ prefixed tables)
$databases['green']['default'] = $base_config;
$databases['green']['default']['prefix'] = 'alt_';

// Active environment (dynamically updated when switching)
$databases['default']['default'] = $base_config;
$databases['default']['default']['prefix'] = ''; // or 'alt_' when green is active
```

The module handles all database credentials automatically, whether you're using DDEV, manual configuration, or other hosting environments.

## Usage

1. **View Environment Status**: Visit the Blue-Green Deployment admin page to see both environments
2. **Sync Database**: Click "Sync from [Active]" on the idle environment to copy the live database
3. **Make Changes**: Work on the idle environment without affecting the live site
4. **Switch Live**: Click "Make [Environment] Live" to instantly switch environments
5. **Rollback**: Switch back just as easily if something goes wrong

## Permissions

- **Administer blue-green deployment**: Full access to all functionality
- **Switch environments**: Can switch which environment is live
- **Sync environments**: Can sync database between environments

## Technical Notes

- Uses Backdrop's database abstraction layer for cross-environment operations
- Integrates with core backup module for pre-switch backups
- Tracks last sync time for each environment
- Validates environment configuration on runtime
- Blue environment: unprefixed tables (original installation)
- Green environment: `alt_` prefixed tables (alternate copy)
- Settings managed in `settings.bluegreen.php` (auto-generated)

## Workflow Example

1. Site is live on Blue environment
2. Sync Blue → Green to get current data
3. Test updates/changes on Green
4. Switch to Green (Green is now live)
5. If problems occur, switch back to Blue instantly

## Support

See the project repository for issues and documentation.

