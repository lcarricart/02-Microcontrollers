# udp_listen_log_min.ps1 — first packet shows sender; afterwards print ONLY payload text
param(
  [int]$Port = 55055,
  [string]$LocalIP = "0.0.0.0",     # bind to specific local IPv4 (e.g., 192.168.7.1) or 0.0.0.0 for all
  [switch]$Echo,                    # reply back with the same payload
  [string]$LogPath = $null,         # defaults to Desktop\udp_log_YYYYMMDD_HHMMSS.txt
  [switch]$UtcTimestamps            # timestamps only used on the very first line
)

# ---- setup log file ----------------------------------------------------------
if (-not $LogPath) {
  $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
  $LogPath = Join-Path $env:USERPROFILE "Desktop\udp_log_$stamp.txt"
}
$logDir = Split-Path -Parent $LogPath
if (-not (Test-Path $logDir)) { New-Item -ItemType Directory -Path $logDir | Out-Null }
$encoding = New-Object System.Text.UTF8Encoding($false)   # UTF-8, no BOM
$sw = New-Object System.IO.StreamWriter($LogPath, $true, $encoding)

# ---- bind socket -------------------------------------------------------------
$ipAddr  = [System.Net.IPAddress]::Parse($LocalIP)
$localEP = [System.Net.IPEndPoint]::new($ipAddr, $Port)

$udp = [System.Net.Sockets.UdpClient]::new()
$udp.Client.SetSocketOption([System.Net.Sockets.SocketOptionLevel]::Socket,
                            [System.Net.Sockets.SocketOptionName]::ReuseAddress, $true)
$udp.Client.Bind($localEP)

$remoteEP = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, 0)
Write-Host ("Listening on {0}:{1} (Echo: {2}) — logging to {3} — Ctrl+C to stop." -f $LocalIP, $Port, $Echo.IsPresent, $LogPath)

# ---- receive loop ------------------------------------------------------------
$first = $true
try {
  while ($true) {
    try {
      $bytes = $udp.Receive([ref]$remoteEP)
      if ($null -eq $bytes) { continue }

      if ($Echo) { [void]$udp.Send($bytes, $bytes.Length, $remoteEP) }

      $text = [Text.Encoding]::UTF8.GetString($bytes)

      if ($first) {
        $ts = if ($UtcTimestamps) { (Get-Date).ToUniversalTime().ToString("yyyy-MM-dd HH:mm:ss.fff 'UTC'") }
              else                 { (Get-Date).ToString("yyyy-MM-dd HH:mm:ss.fff") }
        $hdr = "[{0}] first packet from {1}:{2}" -f $ts, $remoteEP.Address, $remoteEP.Port
        Write-Host $hdr
        $sw.WriteLine($hdr)
        $first = $false
      }

      # After the first packet: print ONLY the payload line (no extra info)
      Write-Host $text
      $sw.WriteLine($text)
      $sw.Flush()
    } catch {
      Write-Warning $_.Exception.Message
      Start-Sleep -Milliseconds 100
    }
  }
}
finally {
  try { $sw.Flush(); $sw.Dispose() } catch {}
  try { $udp.Close() } catch {}
  Write-Host "`nLog saved at $LogPath"
}
