cd ..
git init
git add .
git commit -m "first commit"
set /p link=
git remote add origin %link%
git push -u origin master