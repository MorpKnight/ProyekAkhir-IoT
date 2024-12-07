require('dotenv').config();
const express = require('express');
const multer = require('multer');
const path = require('path');
const fs = require('fs');

const app = express();
const port = process.env.PORT || 3000;

// Buat direktori "uploads" jika belum ada
const uploadDir = './uploads';
if (!fs.existsSync(uploadDir)) {
    fs.mkdirSync(uploadDir, { recursive: true });
}

// Konfigurasi multer untuk menyimpan file dengan nama unik
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        cb(null, uploadDir);
    },
    filename: (req, file, cb) => {
        const timestamp = new Date().toISOString().replace(/[:]/g, '-'); // Ganti ':' agar nama file valid
        cb(null, `${timestamp}_${file.originalname}`);
    },
});

const upload = multer({
    storage: storage,
    limits: {
        fileSize: 500000, // Maksimal ukuran file 500 KB
    },
    fileFilter: (req, file, cb) => {
        const fileTypes = /jpeg|jpg|png|gif/;
        const extname = fileTypes.test(path.extname(file.originalname).toLowerCase());
        const mimetype = fileTypes.test(file.mimetype);

        if (extname && mimetype) {
            return cb(null, true);
        } else {
            cb(new Error('Only .jpg, .jpeg, .png, and .gif files are allowed!'));
        }
    },
});

// Endpoint untuk upload file
app.post('/upload', upload.single('imageFile'), (req, res) => {
    if (req.file) {
        res.status(200).send(`File ${req.file.originalname} has been uploaded successfully.`);
    } else {
        res.status(400).send('No file uploaded or invalid file type.');
    }
});

// Error handling middleware
app.use((err, req, res, next) => {
    if (err instanceof multer.MulterError) {
        res.status(400).send(`Multer Error: ${err.message}`);
    } else if (err) {
        res.status(400).send(`Error: ${err.message}`);
    } else {
        next();
    }
});

// Jalankan server
app.listen(port, () => {
    console.log(`Server is running at http://localhost:${port}`);
});
