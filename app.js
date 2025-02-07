const express = require('express');
const bodyParser = require('body-parser');
const bcrypt = require('bcrypt');
const cors = require('cors');
const { MongoClient, ObjectId } = require('mongodb');


const app = express();
app.use(bodyParser.json());
app.use(cors());

// MongoDB connection
const uri = 'mongodb://localhost:27017'; // Replace with your MongoDB URI if needed
const dbName = 'parkingSystem';
let db;

MongoClient.connect(uri, { useUnifiedTopology: true })
    .then(client => {
        db = client.db(dbName);
        console.log('Connected to MongoDB');
    })
    .catch(err => {
        console.error('Failed to connect to MongoDB', err);
    });

// Login endpoint
app.post('/login', async (req, res) => {
    const { email, password } = req.body;

    try {
        const user = await db.collection('users').findOne({ email: email });
        console.log(user);
        if (!user) {
            return res.json({ success: false, message: 'User not found' });
        }

        var isMatch = false;
        bcrypt.compare(password, user.password).then(isMatch = true)
       
        if (!isMatch) {
            return res.json({ success: false, message: 'Invalid credenials' });
        }

        res.json({ success: true, message: 'Login successful' });
    } catch (err) {
        console.error(err);
        res.status(500).json({ success: false, message: 'Server error' });
    }
});

// Booking endpoint
app.post('/book-slot', async (req, res) => {
    const { slotId } = req.body;

    try {
        const result = await db.collection('slots').findOneAndUpdate(
            { _id: new ObjectId(slotId), isBooked: false },
            { $set: { isBooked: true } },
            { returnOriginal: false }
        );

        if (!result.value) {
            return res.json({ success: false, message: 'Slot already booked' });
        }

        res.json({ success: true, message: 'Slot booked successfully' });
    } catch (err) {
        console.error(err);
        res.status(500).json({ success: false, message: 'Server error' });
    }
});

// Get available slots
app.get('/slots', async (req, res) => {
    try {
        const slots = await db.collection('slots').find({ isBooked: false }).toArray();
        res.json(slots);
    } catch (err) {
        console.error(err);
        res.status(500).json({ success: false, message: 'Server error' });
    }
});

// Start the server
app.listen(3001, () => {
    console.log('Server running on port 3001');
});
