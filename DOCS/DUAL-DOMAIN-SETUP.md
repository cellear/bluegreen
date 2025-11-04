# Dual Domain Setup for Blue-Green Deployment

This feature allows you to access both environments simultaneously using different domains.

## How It Works

### Two Domains

1. **Main Domain** (`bluegreen-v3.ddev.site`) - Always shows the **LIVE** environment
   - When Blue is active → shows Blue
   - When Green is active → shows Green
   
2. **Dev Domain** (`dev.bluegreen-v3.ddev.site`) - Always shows the **DEV** environment
   - When Blue is active → shows Green (the idle one)
   - When Green is active → shows Blue (the idle one)

### Automatic Switching

When you switch environments using the admin interface:
- The main domain automatically routes to the newly active environment
- The dev domain automatically routes to the newly inactive environment
- No manual configuration needed!

## DDEV Configuration

The module setup automatically configures DDEV with:
```yaml
additional_hostnames:
  - dev
```

This creates `dev.bluegreen-v3.ddev.site` alongside the main domain.

## Domain-Based Routing

The `settings.bluegreen.php` file includes logic that:
1. Detects which domain is being accessed (`$_SERVER['HTTP_HOST']`)
2. Reads the active environment from the state table
3. Routes to the appropriate database:
   - Main domain → Active environment (Live)
   - Dev domain → Inactive environment (Dev)

## Use Cases

### Content Editors
- Edit content on dev domain
- Preview changes safely
- Switch to make live when ready
- Both domains remain accessible

### Developers  
- Test changes on dev domain
- Live site stays stable on main domain
- Quick switching between environments
- Easy comparison of versions

### Demonstrations
- Show live site on main domain
- Show upcoming changes on dev domain
- Switch live in real-time
- Instant rollback if needed

## Technical Details

### State Table Query
The routing logic queries the `state` table directly using PDO to avoid bootstrap issues:
```php
$pdo = new PDO(...);
$stmt = $pdo->query("SELECT value FROM state WHERE name = 'bluegreen_active_environment'");
$active_env = unserialize($row['value']);
```

### Shared State
Because the `state` table is shared between environments, both domains read the same active environment setting, ensuring consistent routing.

### Production Setup
For production environments:
1. Configure your web server (Apache/Nginx) with two domains
2. Point both to the same Backdrop installation
3. The settings.bluegreen.php file handles the routing automatically

Example Nginx configuration:
```nginx
server {
    server_name example.com;
    # ... your configuration ...
}

server {
    server_name dev.example.com;
    # ... same configuration ...
}
```

The routing logic works the same way in production as it does in DDEV.

