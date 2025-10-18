# udp_listen.ps1 — robust UDP listener (print-only by default; add -Echo to echo back)
param(
  [int]$Port = 55055,
  [string]$LocalIP = "0.0.0.0",  # bind to a specific local IPv4 (e.g., 192.168.7.1) or 0.0.0.0 for all
  [switch]$Echo
)

$ipAddr  = [System.Net.IPAddress]::Parse($LocalIP)
$localEP = [System.Net.IPEndPoint]::new($ipAddr, $Port)

$udp = [System.Net.Sockets.UdpClient]::new()
$udp.Client.SetSocketOption([System.Net.Sockets.SocketOptionLevel]::Socket,
                            [System.Net.Sockets.SocketOptionName]::ReuseAddress, $true)
$udp.Client.Bind($localEP)

$remoteEP = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, 0)
Write-Host ("Listening on {0}:{1} (Echo: {2}) — Ctrl+C to stop." -f $LocalIP, $Port, $Echo.IsPresent)

while ($true) {
  try {
    $bytes = $udp.Receive([ref]$remoteEP)
    if ($null -eq $bytes) { continue }
    if ($Echo) { [void]$udp.Send($bytes, $bytes.Length, $remoteEP) }
    $text = [Text.Encoding]::UTF8.GetString($bytes)
    Write-Host ("from {0}:{1} -> {2}" -f $remoteEP.Address, $remoteEP.Port, $text)
  } catch {
    Write-Warning $_.Exception.Message
    Start-Sleep -Milliseconds 100
  }
}
