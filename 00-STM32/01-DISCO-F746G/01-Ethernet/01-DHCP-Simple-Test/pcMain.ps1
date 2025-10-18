# Listen on UDP port 5005 and print incoming data
$port = 5005
$udp  = New-Object System.Net.Sockets.UdpClient($port)
$ep   = New-Object System.Net.IPEndPoint([System.Net.IPAddress]::Any,0)
Write-Host "Listening on UDP $port..."
while ($true) {
  $bytes = $udp.Receive([ref]$ep)
  $text  = [Text.Encoding]::UTF8.GetString($bytes)
  Write-Host ("from {0}:{1} -> {2}" -f $ep.Address, $ep.Port, $text)
}