cd ..

git add .

set /p CommitMessage= 

git commit -m "%CommitMessage%"

git push