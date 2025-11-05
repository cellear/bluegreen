# DDEV Multi-Domain Configuration for Blue-Green Deployment

## Quick Start

**For automated setup, see [DDEV-SETUP.md](DDEV-SETUP.md)** - includes a setup script that handles everything automatically!

## Overview
This document provides technical details about how DDEV multi-domain configuration works with the Blue-Green deployment module, allowing seamless switching between environments based on the domain accessed.

## Domain Strategy
- **Production domain**: `{project-name}.ddev.site` → routes to active environment (blue or green)
- **Development domain**: `dev.{project-name}.ddev.site` → routes to idle environment

Example with project named "bluegreen-v4":
- Main: `bluegreen-v4.ddev.site`
- Dev: `dev.bluegreen-v4.ddev.site`

## DDEV Configuration

### 1. Update `.ddev/config.yaml`

Add the development domain to the `additional_fqdns` setting:

```yaml
additional_fqdns:
  - dev.{project-name}.ddev.site
```

Replace `{project-name}` with your actual DDEV project name.

### 2. Restart DDEV

After modifying the configuration, restart DDEV to apply changes:

```bash
ddev restart
```

DDEV will automatically:
- Update the local hosts file
- Configure the web server to respond to both domains
- Generate SSL certificates for both domains

## Backdrop Configuration

### settings.php Domain Routing

Add domain-based routing logic in `settings.php` to automatically select the correct database based on the incoming domain:

```php
// Blue-Green domain-based environment routing
if (isset($_SERVER['HTTP_HOST'])) {
  $active_env = state_get('bluegreen_active_environment', 'blue');
  $idle_env = ($active_env === 'blue') ? 'green' : 'blue';
  
  // Check if this is the dev domain
  if (strpos($_SERVER['HTTP_HOST'], 'dev.') === 0) {
    // Dev domain → idle environment
    $databases['default'] = $databases[$idle_env];
  } else {
    // Production domain → active environment
    $databases['default'] = $databases[$active_env];
  }
}
```

**Note:** This logic needs to be placed after the `$databases` array is defined but before DDEV settings are loaded.

## Testing the Configuration

### Verify DNS Resolution
```bash
# Should resolve to 127.0.0.1 (or your DDEV IP)
ping bluegreen-v2.ddev.site
ping dev.bluegreen-v2.ddev.site
```

### Test in Browser
1. Visit `https://bluegreen-v2.ddev.site` → should show active environment
2. Visit `https://dev.bluegreen-v2.ddev.site` → should show idle environment
3. Switch environments in the module
4. Refresh both URLs → domains should now point to opposite environments

### Verify Database Connection
Check which database prefix is being used:
- Admin toolbar should show environment color
- Database queries should use the appropriate prefix (blue_ or green_)

## Production Deployment Notes

This multi-domain configuration is specific to DDEV for local development. For production deployment:

1. **DNS Configuration**: Point both production and dev subdomains to your server
2. **Web Server Config**: Configure Apache/Nginx virtual hosts for both domains
3. **SSL Certificates**: Obtain certificates for both domains
4. **settings.php**: Use the same domain-based routing logic

## Troubleshooting

### Domain Not Resolving
- Run `ddev restart`
- Check `.ddev/config.yaml` for typos
- Verify hosts file was updated (may require admin/sudo permissions)

### Wrong Database Selected
- Check the state variable: `state_get('bluegreen_active_environment')`
- Verify `$_SERVER['HTTP_HOST']` value matches expected domain
- Review settings.php logic for correct environment mapping

### SSL Certificate Errors
- Run `ddev restart` to regenerate certificates
- Clear browser SSL cache
- Check DDEV version supports additional_fqdns (v1.16+)

## Future Enhancements

Potential improvements to consider:
- Cookie domain sharing between prod/dev domains for SSO
- Automatic subdomain routing for multiple environments
- Environment-specific base URLs in Backdrop config

