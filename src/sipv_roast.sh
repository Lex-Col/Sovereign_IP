#!/bin/bash
TARGET_PID=$(shizuku-shell -c "pidof com.sipv")

if [ -z "$TARGET_PID" ]; then
    echo "[-] Oracle not found."
else
    echo "[!] Triggering Roast Protocol for PID $TARGET_PID..."
    shizuku-shell -c "kill -SIGUSR1 $TARGET_PID"
    echo "[✓] Cleaned."
fi
